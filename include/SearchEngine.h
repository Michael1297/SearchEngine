#pragma once
#include <string>
#include <mutex>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <ThreadPool/ThreadPool.h>
#include "GumboAPI.h"
#include "HttpTool.h"
#include "Stemming.h"
#include "SQL_database.h"
#include "StopWord.h"
#include "GumboAPI.h"

class SearchEngine{
    bool now_indexing = false;
    std::mutex mutex;
    HttpTool domain;                                        //работа с адресом страницы
    Stemming stemming;                                      //стемминг слова
    SQL_database database;                                  //база данных
    StopWord stopWord;                                      //стоп слова
    std::unordered_set<std::string> buffer_sites;           //сайты на индексацию
    void indexing(std::string current_link, bool single, ThreadPool* thread_pool);   //индексация
    void indexing_words(GumboAPI& html_parse, int page_id);              //индексация слов
    void indexing_page(GumboAPI& html_parse, ThreadPool* thread_pool);   //индексация ссылок
    void buffer_insert(std::string& link);                  //добавить страницу в буфер
    void buffer_erase(std::string& current_link);           //удалить страницу из буфера
    void parsing(std::unordered_set<std::string>& worlds, std::string text, bool is_word);  //парсинг запроса

public:
    nlohmann::json startIndexing();
    nlohmann::json startIndexing(std::string queurls);
    nlohmann::json stopIndexing();
    nlohmann::json status();
    nlohmann::json search(const std::string& query, int offset, int limit);
};