#include "SearchEngine.h"
#include <thread>
#include <vector>
#include <map>
#include <cpr/cpr.h>
#include "GumboAPI.h"
#include <Config.h>

void SearchEngine::buffer_erase(std::string& current_link) {
    mutex.lock();
    buffer_sites.erase(current_link);
    mutex.unlock();
}

void SearchEngine::parsing(std::unordered_set<std::string>& worlds, const std::string& text, char symbol) {
    std::stringstream parse;
    parse << text;
    while (true){
        std::string word;
        std::getline(parse, word, symbol);
        if(!word.empty()){
            worlds.insert(stemming.word_stemming(word));
        } else break;
    }
}

void SearchEngine::indexing(std::string current_link){
    cpr::Response r = cpr::Get(cpr::Url(current_link));
    if(r.status_code == 404 || r.status_code == 500){
        try{
            database.insert_page(domain.getPath(current_link), r.status_code, "");
        }
        catch (...){
            //TODO когда будут добавлены логи пометить в них скип проблематичного сайта при добавлении в бд
        }
        this->buffer_erase(current_link);           //удалить страницу из буфера
        return;
    }

    auto path = domain.getPath(current_link);
    int page_id;

    try{
        database.insert_page(path, r.status_code, r.text);     //добавить страницу в бд
        page_id = database.page_id(path);          //сохранить id добавленной страницы
        this->buffer_erase(current_link);           //удалить страницу из буфера
    }
    catch (...){
        //TODO когда будут добавлены логи пометить в них скип проблематичного сайта при добавлении в бд
        this->buffer_erase(current_link);           //удалить страницу из буфера
        return;
    }

    GumboAPI html_parse(r.text);
    html_parse.get_links([this](std::string links){     //получить ссылки со страницы
        mutex.lock();
        bool now_index = now_indexing;
        mutex.unlock();
        if(domain.is_ownLink(links) && now_index) {     //игнорировать ссылки сторонних сайтов
            try{
                if(database.page_id(domain.getPath(links)) == 0) buffer_sites.insert(links);
            }
            catch (...){
                //TODO когда будут добавлены логи пометить в них скип проблематичного сайта
            }
        }
    });

    std::map<std::string, int> buffer_words;    //буфер слов
    html_parse.get_words([this, &buffer_words](std::string out){
        buffer_words[stemming.word_stemming(out)]++;    //добавление слов в буфер
    });

    //добавление слов в бд
    for(auto& word : buffer_words){
        mutex.lock();
        bool now_index = now_indexing;
        mutex.unlock();
        if(!now_index) break;
        try{
            if(auto word_id = database.word_id(word.first); word_id > 0){   //слово присутствует в бд
                database.update_word(word.first);   //увеличить frequency на 1 в бд
                database.insert_search_index(page_id, word_id, word.second);
            } else{
                database.insert_word(word.first);   //добавить слово в бд
                database.insert_search_index(page_id, database.word_id(word.first), word.second);
            }
        }
        catch (...){
            //TODO когда будут добавлены логи пометить в них скип проблематичного слова
        }
    }
}

nlohmann::json SearchEngine::startIndexing() {
    nlohmann::json status;
    if(now_indexing){
        status["result"] = false;
        status["error"] = "Индексация уже запущена";
        return status;
    }
    now_indexing = true;
    database.create();
    buffer_sites.insert(Config::Instance().start_page);
    while (!buffer_sites.empty()){
        std::vector<std::thread> threads;
        for(auto current_link : buffer_sites) {
            threads.emplace_back(&SearchEngine::indexing, this, current_link);
            if(threads.size() > 100) break; //лимит 100 потоков
        }
        for(auto& thread : threads) thread.join();
    }
    status["result"] = true;
    now_indexing = false;
    return status;
}

nlohmann::json SearchEngine::startIndexing(std::string queurls) {
    std::unordered_set<std::string> sites;
    this->parsing(sites, queurls);

    for(auto site : sites){
        if(!domain.is_ownLink(site)) continue;
        auto path = domain.getPath(site);
        database.erase_page(path);
        cpr::Response r = cpr::Get(cpr::Url(site));
        if(r.status_code == 404 || r.status_code == 500){
            try{
                database.insert_page(path, r.status_code, "");
            }
            catch (...){
                //TODO когда будут добавлены логи пометить в них скип проблематичного сайта при добавлении в бд
            }
            continue;
        } else{
            try{
                database.insert_page(path, r.status_code, r.text);
            }
            catch (...){
                continue;
                //TODO когда будут добавлены логи пометить в них скип проблематичного сайта при добавлении в бд
            }
        }

        GumboAPI html_parse(r.text);
        std::map<std::string, int> buffer_words;    //буфер слов
        html_parse.get_words([this, &buffer_words](std::string out){
            buffer_words[stemming.word_stemming(out)]++;    //добавление слов в буфер
        });
        auto page_id = database.page_id(path);          //сохранить id добавленной страницы
        //добавление слов в бд
        for(auto& word : buffer_words){
            try{
                if(auto word_id = database.word_id(word.first); word_id > 0){   //слово присутствует в бд
                    database.update_word(word.first);   //увеличить frequency на 1 в бд
                    database.insert_search_index(page_id, word_id, word.second);
                } else{
                    database.insert_word(word.first);   //добавить слово в бд
                    database.insert_search_index(page_id, database.word_id(word.first), word.second);
                }
            }
            catch (...){
                //TODO когда будут добавлены логи пометить в них скип проблематичного слова
            }

        }

    }
    return {{"result", true}};
}

nlohmann::json SearchEngine::stopIndexing() {
    mutex.lock();
    now_indexing = false;
    mutex.unlock();
    nlohmann::json status = {{"result", "stop"}};
    return status;
}

nlohmann::json SearchEngine::status() {
    nlohmann::json status;
    status["status"] = "OK";
    status["pages_count"] = database.size("page");
    status["words_count"] = database.size("search_index");
    status["index_size"] = database.size("word");
    return status;
}

nlohmann::json SearchEngine::search(std::string query, int offset, int limit) {
    nlohmann::json status;
    if(query.empty()){
        status["result"] = false;
        status["error"] = "Задан пустой поисковый запрос";
        return status;
    }
    while(query.front() == '+' && !query.empty()) query.erase(query.begin()); //добавлено на случай если запрос некорректный и 1 символом будет +
    std::unordered_set<std::string> worlds;
    this->parsing(worlds, query);

    auto search_result = database.search(worlds);

    if(search_result.empty()){
        status["result"] = false;
        status["error"] = "Не найдено";
    } else{
        status["result"] = true;
        status["count"] = search_result.size();
        for(int i = offset; i < limit + offset && i < search_result.size(); i++){
            status["data"].push_back(search_result[i]);
        }
    }

    return status;
}
