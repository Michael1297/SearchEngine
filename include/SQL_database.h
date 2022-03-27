#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include <nanodbc/nanodbc.h>
#include <nlohmann/json.hpp>

class SQL_database{
    std::unique_ptr<nanodbc::connection> database;
    void general(std::unordered_set<int>& main, std::unordered_set<int>& other); //убрать из 1 массива уникальные значения
public:
    SQL_database();
    void connection();  //подключение к бд
    void create();  //создать таблицу
    void drop(std::string table);    //удалить таблицу
    void insert_page(std::string path, int code, std::string content);  //добавить сайт в бд
    void insert_word(std::string value);    //добавить слово в бд
    void insert_search_index(int page_id, int word_id, float rank);
    void erase_page(std::string path);
    int page_id(std::string path);          //id страницы
    int word_id(std::string value);         //id слова
    void update_word(std::string value);    //увеличить frequency на 1
    int size(std::string table);            //размер таблицы
    float get_relevance(int page_id, std::string world);    //получить релевантность слова на странице
    nlohmann::json search(std::unordered_set<std::string>& worlds); //поиск слов в бд
};
