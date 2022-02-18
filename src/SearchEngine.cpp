#include "SearchEngine.h"
#include <iostream>
#include <sstream>
#include <cpr/cpr.h>

std::string SearchEngine::StemEnglishFunc(std::string str){
    std::wstring stem_str (str.begin(), str.end());
    StemEnglish(stem_str);
    std::string out  (stem_str.begin(), stem_str.end());
    return out;
}

void SearchEngine::search_for_links(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) return;

    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        if(std::regex_match(href->value, own_link)) std::cout << href->value << std::endl;
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        this->search_for_links(static_cast<GumboNode*>(children->data[i]));
    }
}

void SearchEngine::clean_text(GumboNode* node, std::string str) {
    std::stringstream text;
    if (node->type == GUMBO_NODE_TEXT) {
        text << node->v.text.text;
    } else if (node->type == GUMBO_NODE_ELEMENT &&
               node->v.element.tag != GUMBO_TAG_SCRIPT &&
               node->v.element.tag != GUMBO_TAG_STYLE) {
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            clean_text((GumboNode*) children->data[i], str);
            text << str;
        }
    }

    while(true){
        std::string out;
        text >> out;
        if(out.empty()) break;
        for (auto &i: out) i = std::tolower(i);     //делает все буквы строчными
        if(!std::regex_match(out, not_word)) words.insert(StemEnglishFunc(out));
    }
}

void SearchEngine::startIndexing() {
    cpr::Response r = cpr::Get(cpr::Url(link));
    GumboOutput* output = gumbo_parse(r.text.c_str());
    search_for_links(output->root);
    //clean_text(output->root);
    for(auto& word : words) std::cout << word << "\n";
    gumbo_destroy_output(&kGumboDefaultOptions, output);
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
