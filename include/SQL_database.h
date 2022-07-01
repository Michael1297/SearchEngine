#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include <nanodbc/nanodbc.h>
#include <nlohmann/json.hpp>

class SQL_database{
    std::unique_ptr<nanodbc::connection> database;

public:
    SQL_database();  //подключение к бд
    ~SQL_database();
    void create();      //создать таблицу
    void drop(const std::string& table);           //удалить таблицу
    void insert_page(const std::string& path, int code, const std::string& content);  //добавить сайт в бд
    void insert_word(const std::string& value);    //добавить слово в бд
    void insert_search_index(int page_id, int word_id, float rank); //добавить search_index в бд
    void erase_page(std::string path);      //удалить страницу из бд
    int page_id(const std::string& path);          //id страницы
    int word_id(const std::string& value);         //id слова
    void update_word(const std::string& value);    //увеличить frequency на 1
    int size(const std::string& table);            //размер таблицы
    [[maybe_unused]] std::string get_page_content(const std::string& path); //получить код страница
    nlohmann::json search(std::unordered_set<std::string>& worlds); //поиск слов в бд
};
