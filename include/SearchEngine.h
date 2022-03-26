#pragma once
#include <string>
#include <mutex>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include "GumboAPI.h"
#include "HttpTool.h"
#include "Stemming.h"
#include "SQL_database.h"

class SearchEngine{
    bool now_indexing = false;
    std::mutex mutex;
    HttpTool domain;
    std::unordered_set<std::string> buffer_sites;           //сайты на индексацию
    Stemming stemming;                                      //стемминг слова
    SQL_database database;                                  //база данных
    void indexing(std::string current_link);
    void buffer_erase(std::string& current_link);           //удалить страницу из буфера
    void parsing(std::unordered_set<std::string>& worlds, const std::string& text, char symbol = '+');
public:

    nlohmann::json startIndexing();
    nlohmann::json startIndexing(std::string queurls);
    nlohmann::json stopIndexing();
    nlohmann::json status();
    nlohmann::json search(std::string query, int offset, int limit);
};