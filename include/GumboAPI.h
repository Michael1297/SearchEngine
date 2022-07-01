#pragma once
#include <string>
#include <functional>
#include "gumbo.h"

class GumboAPI{
    GumboOutput* output = nullptr;

public:
    explicit GumboAPI(const std::string& code);
    void get_links(const std::function<void(std::string)>& function, GumboNode* node = nullptr);       //выдает ссылки со страницы
    void get_fragments(const std::function<void(std::string)>& function, GumboNode* node = nullptr);   //выдает фрагменты текста
    void get_words(std::function<void(std::string)> function);  //выдает слова
    const std::string find_title(); //выдает заголовок
    ~GumboAPI();
};
