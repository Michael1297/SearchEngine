#pragma once
#include <string>
#include <functional>
#include "gumbo.h"

class GumboAPI{
    GumboOutput* output = nullptr;
    void get_links(const std::function<void(std::string)>& function, GumboNode* node);       //выдает ссылки со страницы
    void get_fragments(const std::function<void(std::string)>& function, GumboNode* node);   //выдает фрагменты текста

public:
    explicit GumboAPI(const std::string& code);
    void get_links(const std::function<void(std::string)>& function);       //выдает ссылки со страницы
    void get_fragments(const std::function<void(std::string)>& function);   //выдает фрагменты текста
    void get_words(const std::function<void(std::string)>& function);       //выдает слова
    std::string find_title(); //выдает заголовок
    ~GumboAPI();
};
