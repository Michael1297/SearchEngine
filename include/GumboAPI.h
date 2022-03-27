#pragma once
#include <string>
#include <functional>
#include "gumbo.h"

class GumboAPI{
    GumboOutput* output = nullptr;

public:
    GumboAPI(std::string code);
    void get_links(std::function<void(std::string)> function, GumboNode* node = nullptr);
    void get_fragments(std::function<void(std::string)> function, GumboNode* node = nullptr);
    void get_words(std::function<void(std::string)> function);
    const std::string find_title();
    ~GumboAPI();
};
