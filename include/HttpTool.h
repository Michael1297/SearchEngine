#ifndef SEARCHENGINE_HTTPTOOL_H
#define SEARCHENGINE_HTTPTOOL_H

#pragma once
#include <string>
#include <memory>
#include <regex>

class HttpTool{
    std::string domain;
    std::regex own_link_regex;
public:
    HttpTool(std::string link);
    bool is_ownLink(std::string& link);
    std::string getDomain();
};

#endif //SEARCHENGINE_HTTPTOOL_H
