#ifndef SEARCHENGINE_SQL_DATABASE_H
#define SEARCHENGINE_SQL_DATABASE_H

#pragma once
#include <memory>
#include <nanodbc/nanodbc.h>

class SQL_database{
    std::string database_name;
    std::unique_ptr<nanodbc::connection> database;
public:
    SQL_database();
    void create();  //создать бд
    void drop();    //удалить бд
    void use();     //использовать созданную бд
    void insert(std::string& site);  //добавить сайт в бд
    void insert(std::string& site, std::string& word);  //добавить слово в бд
    bool contains(std::string& site);   //находится ли сайт в бд
    bool contains(std::string& site, std::string& word);   //находится ли сайт в бд
    void update(std::string& site);   //установить is_indexed true
    void update(std::string& site, std::string& word);  //увеличить quality word на 1
};

#endif //SEARCHENGINE_SQL_DATABASE_H
