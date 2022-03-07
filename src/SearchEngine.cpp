#include "SearchEngine.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cpr/cpr.h>
#include "GumboAPI.h"

std::string SearchEngine::StemEnglishFunc(std::string str){
    std::wstring stem_str (str.begin(), str.end());
    StemEnglish(stem_str);
    std::string out  (stem_str.begin(), stem_str.end());
    return out;
}

void SearchEngine::indexing(std::string current_link){
    cpr::Response r = cpr::Get(cpr::Url(current_link));
    GumboAPI html_parse(r.text);
    html_parse.get_links([this](std::string links){
        if(domain->is_ownLink(links)) std::cout << links << "\n";
    });
    html_parse.get_text([this](std::string out){
        for (auto &i: out) i = std::tolower(i);                 //делает все буквы строчными
        if(!std::regex_match(out, not_word)) words.insert(StemEnglishFunc(out));
    });
}

void SearchEngine::startIndexing() {
    std::unordered_set<std::string> links;
    std::vector<std::thread> threads;
    indexing(domain->getDomain());
    for(auto& word : words) std::cout << word << "\n";
    std::cout << "words.size() " << words.size() << "\n";
}

SearchEngine::SearchEngine(const std::string& link) {
    domain = std::make_unique<HttpTool>(link);
}
