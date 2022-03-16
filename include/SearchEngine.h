#pragma once
#ifndef MAIN_CPP_SEARCHENGINE_H
#define MAIN_CPP_SEARCHENGINE_H

#include <string>
#include <memory>
#include <mutex>
#include <unordered_set>
#include "GumboAPI.h"
#include "HttpTool.h"
#include "Stemming.h"
#include "SQL_database.h"

class SearchEngine{
    std::mutex mutex;
    std::unique_ptr<HttpTool> domain;
    std::unordered_set<std::string> buffer_sites;           //сайты на индексацию
    Stemming stemming;                                      //стемминг слова
    SQL_database database;                                  //база данных
    void indexing(std::string current_link);
public:

    SearchEngine(const std::string& _link);                //ввод ссылки + генерация own_link regex для проверки ссылок
    void startIndexing();
};

#endif //MAIN_CPP_SEARCHENGINE_H