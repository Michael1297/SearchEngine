#include "SearchEngine.h"
#include <thread>
#include <vector>
#include <map>
#include <cpr/cpr.h>
#include "GumboAPI.h"
#include "Config.h"

void SearchEngine::buffer_erase(std::string& current_link) {
    mutex.lock();
    buffer_sites.erase(current_link);
    mutex.unlock();
}

void SearchEngine::buffer_insert(std::string& link) {
    mutex.lock();
    buffer_sites.insert(link);
    mutex.unlock();
}

void SearchEngine::parsing(std::unordered_set<std::string>& worlds, std::string text, bool is_word) {
    text = std::regex_replace(text, std::regex("\\++"), " ");
    std::stringstream parse(text);

    while (!parse.eof()){
        std::string word;
        parse >> word;
        if(is_word){    //список слов
            if(!stopWord.contains(word)) worlds.insert(stemming.word_stemming(word));    //добавить слово в массив
        } else{     //список сайтов
            worlds.insert(word);    //добавить сайт в массив
        }
    }
}

void SearchEngine::indexing(std::string current_link, bool single, ThreadPool* thread_pool){
    cpr::Response r = cpr::Get(cpr::Url(current_link));
    if(r.status_code == 404 || r.status_code == 500){
        database.insert_page(domain.getPath(current_link), r.status_code, "");     //добавить страницу в бд
        this->buffer_erase(current_link);              //удалить страницу из буфера
        return;
    }

    auto path = domain.getPath(current_link);
    database.insert_page(path, r.status_code, r.text);     //добавить страницу в бд
    this->buffer_erase(current_link);              //удалить страницу из буфера
    int page_id = database.page_id(path);          //сохранить id добавленной страницы
    GumboAPI html_parse(r.text);                   //парсинг страницы

    mutex.lock();
    if(!now_indexing){
        mutex.unlock();
        return;
    }
    mutex.unlock();

    if(!single) {   //полная индексация
        html_parse.get_links([this, &thread_pool](std::string links){     //получить ссылки со страницы
            if(domain.is_ownLink(links)) {     //игнорировать ссылки сторонних сайтов
                //индексировать если страницы нет в бд
                if(database.page_id(domain.getPath(links)) == 0 && !buffer_sites.count(links)) {
                    this->buffer_insert(links);
                    thread_pool->enqueue(&SearchEngine::indexing, this, links, false, thread_pool);
                }
            }
        });
    }

    std::map<std::string, int> buffer_words;    //буфер слов
    html_parse.get_words([this, &buffer_words](std::string out){
        if(out = stemming.word_stemming(out); !out.empty()) buffer_words[out]++;    //добавление слов в буфер
    });

    //добавление слов в бд
    for(auto& word : buffer_words){
        mutex.lock();   //мьютекс требуется, чтобы не появились дубликаты
        if(!now_indexing) {     //остановка индексации
            mutex.unlock();
            return;
        } else if(auto word_id = database.word_id(word.first); word_id > 0){   //слово присутствует в бд
            database.update_word(word.first);   //увеличить frequency на 1 в бд
            database.insert_search_index(page_id, word_id, word.second);
        } else{
            database.insert_word(word.first);   //добавить слово в бд
            database.insert_search_index(page_id, database.word_id(word.first), word.second);
        }
        mutex.unlock();
    }
}

nlohmann::json SearchEngine::startIndexing() {
    if(now_indexing){
        nlohmann::json status;
        status["result"] = false;
        status["error"] = "Indexing is already running";
        return status;
    }

    now_indexing = true;
    buffer_sites.clear();
    database.create();

    auto* thread_pool = new ThreadPool(std::thread::hardware_concurrency());
    this->indexing(Config::Instance().start_page, false, thread_pool);
    delete thread_pool;

    now_indexing = false;
    return {{"result", true}};
}

nlohmann::json SearchEngine::startIndexing(std::string queurls) {
    if(now_indexing){
        nlohmann::json status;
        status["result"] = false;
        status["error"] = "Indexing is already running";
        return status;
    }

    std::unordered_set<std::string> sites;
    this->parsing(sites, queurls, false);
    now_indexing = true;
    ThreadPool thread_pool(std::thread::hardware_concurrency());

    for(auto site : sites){
        if(domain.is_ownLink(site)){    //индексировать только страницы принадлежащие сайту
            database.erase_page(domain.getPath(site));  //удалить страницу из бд
            thread_pool.enqueue(&SearchEngine::indexing, this, site, true, &thread_pool);
        }
    }

    now_indexing = false;
    return {{"result", true}};
}

nlohmann::json SearchEngine::stopIndexing() {
    nlohmann::json status;
    mutex.lock();
    if(now_indexing){
        now_indexing = false;
        status["result"] = "Stop";
    } else{
        status["result"] = "Error";
        status["error"] = "No indexing running!";
    }
    mutex.unlock();
    return status;
}

nlohmann::json SearchEngine::status() {
    nlohmann::json status;
    try{
        status["pages_count"] = database.size("page");
        status["words_count"] = database.size("search_index");
        status["index_size"] = database.size("word");
        status["status"] = "OK";
    }
    catch (...) {
        status["status"] = "Error";
        status["error"] = "Tables not found in database!";
    }
    return status;
}

nlohmann::json SearchEngine::search(std::string query, int offset, int limit) {
    nlohmann::json status;
    if(query.empty()){
        status["result"] = false;
        status["error"] = "An empty search query is set!";
        return status;
    }
    while(query.front() == '+' && !query.empty()) query.erase(query.begin()); //добавлено на случай если запрос некорректный и 1 символом будет +
    std::unordered_set<std::string> worlds;
    this->parsing(worlds, query, true);

    auto search_result = database.search(worlds);

    if(search_result.empty()){
        status["result"] = false;
        status["error"] = "Not found!";
    } else{
        status["result"] = true;
        status["count"] = search_result.size();
        for(int i = offset; i < limit + offset && i < search_result.size(); i++){
            status["data"].push_back(search_result[i]);
        }
    }

    return status;
}
