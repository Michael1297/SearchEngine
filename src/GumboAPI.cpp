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

void GumboAPI::get_fragments(std::function<void(std::string)> function, GumboNode *node) {
    if(node == nullptr) node = output->root;
    assert(node != nullptr);
    if (node->type == GUMBO_NODE_TEXT) {    //получение текста
        function(node->v.text.text);    //вывод текста
    } else if (node->type == GUMBO_NODE_ELEMENT &&
               node->v.element.tag != GUMBO_TAG_SCRIPT &&
               node->v.element.tag != GUMBO_TAG_STYLE) {    //проход по элементам для получения текста из них
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            this->get_fragments(function, (GumboNode*) children->data[i]);
        }
    }
}

void GumboAPI::get_words(std::function<void(std::string)> function) {
    this->get_fragments([&function](std::string out){
        std::stringstream text;
        text << out;
        while(true){                //парсинг текста
            std::string out;
            text >> out;
            if(out.empty()) break;  //если текст закончился
            function(out);     //вывод слов
        }
    });
}

const std::string GumboAPI::find_title() {
    const GumboNode* root = output->root;
    assert(root->type == GUMBO_NODE_ELEMENT);
    assert(root->v.element.children.length >= 2);

    const GumboVector* root_children = &root->v.element.children;
    GumboNode* head = NULL;
    for (int i = 0; i < root_children->length; ++i) {
        GumboNode* child = (GumboNode*) root_children->data[i];
        if (child->type == GUMBO_NODE_ELEMENT &&
            child->v.element.tag == GUMBO_TAG_HEAD) {
            head = child;
            break;
        }
    }
    assert(head != NULL);

    GumboVector* head_children = &head->v.element.children;
    for (int i = 0; i < head_children->length; ++i) {
        GumboNode* child = (GumboNode*) head_children->data[i];
        if (child->type == GUMBO_NODE_ELEMENT &&
            child->v.element.tag == GUMBO_TAG_TITLE) {
            if (child->v.element.children.length != 1) {
                return " ";
            }
            GumboNode* title_text = (GumboNode*) child->v.element.children.data[0];
            assert(title_text->type == GUMBO_NODE_TEXT || title_text->type == GUMBO_NODE_WHITESPACE);
            return title_text->v.text.text;
        }
    }
    return " ";
}

GumboAPI::GumboAPI(std::string code){
    output = gumbo_parse(code.c_str());
}

GumboAPI::~GumboAPI() {
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}