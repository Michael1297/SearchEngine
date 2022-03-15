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
        database.insert_page(domain->getPath(current_link), r.status_code, "");
        return;
    }
    std::string content = r.text;
    auto path = domain->getPath(current_link);
    database.insert_page(path, r.status_code, content);     //добавить страницу в бд
    buffer_sites.erase(current_link);           //удалить страницу из буфера
    auto page_id = database.page_id(path);  //сохранить id добавленной страницы

    GumboAPI html_parse(r.text);
    html_parse.get_links([this](std::string links){     //получить ссылки со страницы
        if(domain->is_ownLink(links)) {     //игнорировать ссылки сторонних сайтов
            //if(database.page_id(domain->getPath(links)) == 0) buffer_sites.insert(links);    //TODO  добавить полученную ссылку в буфер
        }
    });

    std::map<std::string, int> buffer_words;    //буфер слов
    html_parse.get_text([this, &buffer_words](std::string out){
        buffer_words[stemming.word_stemming(out)]++;    //добавление слов в буфер
    });

    //добавление слов в бд
    for(auto& word : buffer_words){
        if(auto word_id = database.word_id(word.first); word_id > 0){   //слово присутствует в бд
            database.update_word(word.first);   //увеличить frequency на 1 в бд
            database.insert_search_index(page_id, word_id, word.second);
        } else{
            database.insert_word(word.first);   //добавить слово в бд
            database.insert_search_index(page_id, database.word_id(word.first), word.second);
        }
    }
}

void SearchEngine::startIndexing() {
    database.create();
    while (!buffer_sites.empty()){
        std::vector<std::thread> threads;
        for(auto current_link : buffer_sites) threads.emplace_back(&SearchEngine::indexing, this, current_link);
        for(auto& thread : threads) thread.join();
    }
}

SearchEngine::SearchEngine(const std::string& link) {
    domain = std::make_unique<HttpTool>(link);
    buffer_sites.insert(link);
}