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
        if(std::regex_match(links, own_link)) std::cout << links << "\n";
    });
    html_parse.get_text([this](std::string out){
        for (auto &i: out) i = std::tolower(i);                 //делает все буквы строчными
        if(!std::regex_match(out, not_word)) words.insert(StemEnglishFunc(out));
    });
}

void SearchEngine::startIndexing() {
    std::unordered_set<std::string> links;
    std::vector<std::thread> threads;
    indexing(link);
    for(auto& word : words) std::cout << word << "\n";
    std::cout << "words.size() " << words.size() << "\n";
}

using namespace std::regex_constants;       // для флага icase - игнорирование регистра в ссылке
SearchEngine::SearchEngine(const std::string& _link) : link(_link) {
    std::regex regex("^https?://\\S+");
    std::string str = _link;
    if (std::regex_match(_link, regex)) str.erase(str.begin(), str.begin() + str.find("//") + 2);
    auto find = str.find('/');
    if (find != std::string::npos) str = str.substr(0, find);
    own_link = std::regex("^/\\S+|https?://" + str + "(/\\S*$|$)", icase);
}
