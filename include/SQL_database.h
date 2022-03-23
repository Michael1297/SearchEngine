#ifndef SEARCHENGINE_SQL_DATABASE_H
#define SEARCHENGINE_SQL_DATABASE_H

#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include <nanodbc/nanodbc.h>
#include <nlohmann/json.hpp>
#include "Config.h"

class SQL_database{
    std::string database_name;
    std::unique_ptr<nanodbc::connection> database;
public:
    SQL_database(std::shared_ptr<Config> config);
    void create();  //создать бд
    void drop();    //удалить бд
    void use();     //использовать созданную бд
    void insert_page(std::string path, int code, std::string content);  //добавить сайт в бд
    void insert_word(std::string value);    //добавить слово в бд
    void insert_search_index(int page_id, int word_id, float rank);
    int page_id(std::string path);          //id страницы
    int word_id(std::string value);         //id слова
    void update_word(std::string value);    //увеличить frequency на 1
    int size(std::string table);            //размер таблицы
    nlohmann::json search(std::unordered_set<std::string>& worlds);
};

#endif //SEARCHENGINE_SQL_DATABASE_H
