#ifndef SEARCHENGINE_HTTPTOOL_H
#define SEARCHENGINE_HTTPTOOL_H

#pragma once
#include <string>
#include <memory>
#include <regex>

class HttpTool{
    std::string domain;
    std::regex own_link_regex;
    std::regex has_domain = std::regex(R"((^https?://|^)(www\.|)\S+\.\S+(/\S*$|$))", std::regex::icase);
public:
    HttpTool(std::string link);
    bool is_ownLink(std::string& link);
    std::string getDomain();
    std::string getPath(std::string link);
    static void escape(std::string& text, char symbol);      //экранирование символов .
};

#endif //SEARCHENGINE_HTTPTOOL_H
