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
    void insert(std::string& site);  //добавить сайт в бд
    bool contains(std::string& site);
};

#endif //SEARCHENGINE_SQL_DATABASE_H
