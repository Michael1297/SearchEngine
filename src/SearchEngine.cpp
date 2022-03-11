#include "SearchEngine.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cpr/cpr.h>
#include "GumboAPI.h"

void SearchEngine::indexing(std::string current_link){
    cpr::Response r = cpr::Get(cpr::Url(current_link));
    GumboAPI html_parse(r.text);
    html_parse.get_links([this](std::string links){
        if(domain->is_ownLink(links)) {
            if(!database.contains(links)) database.insert(links);
            std::cout << links << "\n";
        }
    });

    html_parse.get_text([this](std::string out){
        if(!std::regex_match(out, not_word)) words.insert(stemming.word_stemming(out));
    });
}

void SearchEngine::startIndexing() {
    database.create();
    std::unordered_set<std::string> links;      //TODO удалить
    std::vector<std::thread> threads;
    indexing(domain->getDomain());
    for(auto& word : words) std::cout << word << "\n";
    std::cout << "words.size() " << words.size() << "\n";
}

SearchEngine::SearchEngine(const std::string& link) {
    domain = std::make_unique<HttpTool>(link);
}