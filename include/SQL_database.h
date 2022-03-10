#ifndef SEARCHENGINE_SQL_DATABASE_H
#define SEARCHENGINE_SQL_DATABASE_H

#pragma once
#include <memory>
#include <nanodbc/nanodbc.h>

class SQL_database{
    std::unique_ptr<nanodbc::connection> conn;
public:
    SQL_database();
};

#endif //SEARCHENGINE_SQL_DATABASE_H
