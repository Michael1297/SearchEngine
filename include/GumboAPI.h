#pragma once

#ifndef MAIN_CPP_GUMBOAPI_H
#define MAIN_CPP_GUMBOAPI_H

#include <string>
#include <functional>
#include "gumbo.h"

class GumboAPI{
    GumboOutput* output = nullptr;

public:
    GumboAPI(std::string);
    void get_links(std::function<void(std::string)> function, GumboNode* node = nullptr);
    void get_fragments(std::function<void(std::string)> function, GumboNode* node = nullptr);
    void get_words(std::function<void(std::string)> function);
    const std::string find_title();
    ~GumboAPI();
};

#endif //MAIN_CPP_GUMBOAPI_H
