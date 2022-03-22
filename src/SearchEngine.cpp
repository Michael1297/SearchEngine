#include "SearchEngine.h"
#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <cpr/cpr.h>
#include "GumboAPI.h"

void SearchEngine::indexing(std::string current_link){
    cpr::Response r = cpr::Get(cpr::Url(current_link));
    if(r.status_code == 404 || r.status_code == 500){
        mutex.lock();
        try{
            database->insert_page(domain->getPath(current_link), r.status_code, "");
        }
        catch (...){
            //TODO когда будут добавлены логи пометить в них скип проблематичного сайта при добавлении в бд
        }
        buffer_sites.erase(current_link);           //удалить страницу из буфера
        mutex.unlock();
        return;
    }

    auto path = domain->getPath(current_link);
    mutex.lock();
    try{
        database->insert_page(path, r.status_code, r.text);     //добавить страницу в бд
        buffer_sites.erase(current_link);           //удалить страницу из буфера
    }
    catch (...){
        //TODO когда будут добавлены логи пометить в них скип проблематичного сайта при добавлении в бд
        buffer_sites.erase(current_link);           //удалить страницу из буфера
        mutex.unlock();
        return;
    }
    auto page_id = database->page_id(path);  //сохранить id добавленной страницы
    mutex.unlock();

    GumboAPI html_parse(r.text);
    html_parse.get_links([this](std::string links){     //получить ссылки со страницы
        if(domain->is_ownLink(links)) {     //игнорировать ссылки сторонних сайтов
            mutex.lock();
            try{
                //if(database.page_id(domain->getPath(links)) == 0 && now_indexing) buffer_sites.insert(links);    //TODO  добавить полученную ссылку в буфер
            }
            catch (...){
                //TODO когда будут добавлены логи пометить в них скип проблематичного сайта
            }
            mutex.unlock();
        }
    });

    std::map<std::string, int> buffer_words;    //буфер слов
    html_parse.get_text([this, &buffer_words](std::string out){
        buffer_words[stemming.word_stemming(out)]++;    //добавление слов в буфер
    });

    //добавление слов в бд
    for(auto& word : buffer_words){
        mutex.lock();
        try{

            if(auto word_id = database->word_id(word.first); word_id > 0){   //слово присутствует в бд
                database->update_word(word.first);   //увеличить frequency на 1 в бд
                database->insert_search_index(page_id, word_id, word.second);
            } else{
                database->insert_word(word.first);   //добавить слово в бд
                database->insert_search_index(page_id, database->word_id(word.first), word.second);
            }
        }
        catch (...){
            //TODO когда будут добавлены логи пометить в них скип проблематичного слова
        }
        mutex.unlock();
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
    database->create();
    buffer_sites.insert(config->start_page);
    while (!buffer_sites.empty()){
        std::vector<std::thread> threads;
        for(auto current_link : buffer_sites) threads.emplace_back(&SearchEngine::indexing, this, current_link);
        for(auto& thread : threads) thread.join();
    }
    status["result"] = true;
    now_indexing = false;
    return status;
}

nlohmann::json SearchEngine::stopIndexing() {
    mutex.lock();
    now_indexing = false;
    mutex.unlock();
    nlohmann::json status;
    status["result"] = "stop";
    return status;
}

nlohmann::json SearchEngine::status() {
    nlohmann::json status;
    status["status"] = "OK";
    status["pages_count"] = database->size("page");
    status["words_count"] = database->size("search_index");
    status["index_size"] = database->size("word");
    return status;
}

SearchEngine::SearchEngine(std::shared_ptr<Config> _config) {
    database = std::make_unique<SQL_database>(_config);
    config = _config;
    domain = std::make_unique<HttpTool>(_config->start_page);
}