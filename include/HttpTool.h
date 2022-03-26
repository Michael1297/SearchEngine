#pragma once
#include <string>
#include <memory>
#include <regex>

class HttpTool{
    std::string domain;
    std::regex own_link_regex;
    std::regex has_domain = std::regex(R"((^https?://|^)(www\.|)\S+\.\S+(/\S*$|$))", std::regex::icase);
    std::regex http_regex = std::regex("^https?://\\S+");
    std::regex www_regex = std::regex("^www\\.\\S+");

public:
    HttpTool();
    bool is_ownLink(std::string& link);
    std::string getDomain();
    std::string getDomain(std::string link);
    std::string getPath(std::string link);
    static void escape(std::string& text, char symbol);      //экранирование символов .
};
