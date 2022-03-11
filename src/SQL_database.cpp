#include "SQL_database.h"
#include <sstream>
#include "Config.h"
#include <iostream> //TODO удалить

SQL_database::SQL_database() {
    //  https://docs.microsoft.com/en-us/sql/integration-services/import-export-data/connect-to-a-mysql-data-source-sql-server-import-and-export-wizard?view=sql-server-2017
    //  Driver={MySQL ODBC 5.3 ANSI Driver};Server=<server>;Database=<database>;UID=<user id>;PWD=<password>
    Config config;      //данные из config.ini файла
    database_name = config.database;
    std::stringstream connection_string; //строка подключения к бд

    connection_string
    << "Driver=" << config.driver << ";"
    << "Server=" << config.server << ";"
    << "Port=" << config.port << ";"
    << "UID=" << config.login << ";"
    << "PWD=" << config.password << ";";

    try{
        database = std::make_unique<nanodbc::connection>(NANODBC_TEXT(connection_string.str()));
        std::cout << database->dbms_name() << "\n";//TODO удалить
    }
    catch (std::runtime_error const& e){
        std::cerr << e.what() << std::endl;//TODO удалить
    }
}

void SQL_database::create() {
    this->drop();
    std::stringstream command;

    command << "CREATE DATABASE " << database_name << ";";
    execute(*database, NANODBC_TEXT(command.str()));
    command.str("");

    command << "USE " << database_name << ";";
    execute(*database, NANODBC_TEXT(command.str()));
    command.str("");

    command << "CREATE TABLE sites("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "site TEXT NOT NULL, "
               "is_indexed BOOLEAN NOT NULL DEFAULT false"
               ");";
    execute(*database, NANODBC_TEXT(command.str()));
    command.str("");

    command << "CREATE TABLE words("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "word TEXT NOT NULL, "
               "quality INT NOT NULL DEFAULT 0, "
               "site_id INT NOT NULL, "
               "FOREIGN KEY (site_id) references sites(id) "
               ");";
    execute(*database, NANODBC_TEXT(command.str()));
}

void SQL_database::drop() {
    std::stringstream command;
    command << "DROP DATABASE IF EXISTS " << database_name << ";";
    execute(*database, NANODBC_TEXT(command.str()));
}

void SQL_database::insert(std::string& site) {
    std::stringstream command;
    command << "INSERT INTO sites (site) value (\"" << site << "\");";
    execute(*database, NANODBC_TEXT(command.str()));
}

bool SQL_database::contains(std::string& site) {
    std::stringstream command;
    command << "SELECT * FROM sites WHERE site=\"" << site << "\";";
    auto result = execute(*database, NANODBC_TEXT(command.str()));
    return result.next();
}