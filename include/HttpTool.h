#pragma once
#include <string>
#include <memory>
#include <regex>

class HttpTool{
    std::string domain;
    std::regex own_link_regex;  //ссылка принадлежит домену
    std::regex has_domain = std::regex(R"((^https?://?|^)(www\.|)\S+\.\S+(/\S*$|$))", std::regex::icase);   //наличие ссылки
    std::regex front_url = std::regex("^https?://?(www\\.|)|^www\\.");

public:
    HttpTool();
    bool is_ownLink(std::string& link);                      //проверить принадлежность ссылки к домену
    std::string getDomain();                                 //получить начальный домен
    std::string getDomain(std::string link);                 //получить домен из ссылки
    std::string getPath(std::string link);                   //получить path
    static void escape(std::string& text, char symbol);      //экранирование символов '.'
};
