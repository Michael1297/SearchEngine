#include "GumboAPI.h"
#include <sstream>
#include <cassert>

void GumboAPI::get_links(std::function<void(std::string)> function, GumboNode *node) {
    if(node == nullptr) node = output->root;
    assert(node != nullptr);
    if (node->type != GUMBO_NODE_ELEMENT) return;

    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        function(href->value);                          //выдаче ссылок
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {   //проход по элементам для получения ссылок из них
        this->get_links(function, static_cast<GumboNode*>(children->data[i]));
    }
}

void GumboAPI::get_text(std::function<void(std::string)> function, GumboNode *node) {
    if(node == nullptr) node = output->root;
    assert(node != nullptr);
    std::stringstream text;
    if (node->type == GUMBO_NODE_TEXT) {    //получение текста
        text << node->v.text.text << " ";
    } else if (node->type == GUMBO_NODE_ELEMENT &&
               node->v.element.tag != GUMBO_TAG_SCRIPT &&
               node->v.element.tag != GUMBO_TAG_STYLE) {    //проход по элементам для получения текста из них
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            this->get_text(function, (GumboNode*) children->data[i]);
        }
    }

    while(true){                //парсинг текста
        std::string out;
        text >> out;
        if(out.empty()) break;  //если текст закончился
        function(out);     //вывод слов
    }
}

GumboAPI::GumboAPI(std::string code){
    output = gumbo_parse(code.c_str());
}

GumboAPI::~GumboAPI() {
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}