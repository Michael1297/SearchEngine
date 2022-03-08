#pragma once
#ifndef MAIN_CPP_SEARCHENGINE_H
#define MAIN_CPP_SEARCHENGINE_H

#include <string>
#include <memory>
#include <unordered_set>
#include "GumboAPI.h"
#include "HttpTool.h"
#include "Stemming.h"

class SearchEngine{
    std::unique_ptr<HttpTool> domain;
    std::regex not_word = std::regex("^\\W+$|^\\d+$");  //рандомные символы вместо слова
    std::unordered_set<std::string> words;      //Todo добавлено временно
    Stemming stemming;                                      //стемминг слова
    void indexing(std::string current_link);
public:

    SearchEngine(const std::string& _link);                //ввод ссылки + генерация own_link regex для проверки ссылок
    void startIndexing();
};

#endif //MAIN_CPP_SEARCHENGINE_H