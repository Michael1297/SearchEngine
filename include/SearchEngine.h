#pragma once
#ifndef MAIN_CPP_SEARCHENGINE_H
#define MAIN_CPP_SEARCHENGINE_H

#include <string>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include "GumboAPI.h"
#include "HttpTool.h"
#include "Stemming.h"
#include "SQL_database.h"
#include "Config.h"

class SearchEngine{
    bool now_indexing = false;
    std::mutex mutex;
    std::unique_ptr<HttpTool> domain;
    std::unordered_set<std::string> buffer_sites;           //сайты на индексацию
    Stemming stemming;                                      //стемминг слова
    std::unique_ptr<SQL_database> database;                 //база данных
    std::shared_ptr<Config> config;
    void indexing(std::string current_link);
    void buffer_erase(std::string& current_link);           //удалить страницу из буфера
public:

    SearchEngine(std::shared_ptr<Config> config);                //ввод ссылки + генерация own_link regex для проверки ссылок
    nlohmann::json startIndexing();
    nlohmann::json stopIndexing();
    nlohmann::json status();
};

#endif //MAIN_CPP_SEARCHENGINE_H