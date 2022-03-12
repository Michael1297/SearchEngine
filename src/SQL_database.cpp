#include "SQL_database.h"
#include <fmt/core.h>
#include "Config.h"
#include <iostream> //TODO удалить

SQL_database::SQL_database() {
    Config config;      //данные из config.ini файла
    database_name = config.database;

    //строка подключения к бд
    //  https://docs.microsoft.com/en-us/sql/integration-services/import-export-data/connect-to-a-mysql-data-source-sql-server-import-and-export-wizard?view=sql-server-2017
    //  Driver={MySQL ODBC 8.0 ANSI Driver};Server=<server>;Database=<database>;UID=<user id>;PWD=<password>
    std::string connection_string = fmt::format("Driver={};Server={};Port={};UID={};PWD={};",
                                                config.driver, config.server, config.port, config.login, config.password);

    try{
        database = std::make_unique<nanodbc::connection>(NANODBC_TEXT(connection_string));
        std::cout << database->dbms_name() << "\n";//TODO удалить
    }
    catch (std::runtime_error const& e){
        std::cerr << e.what() << std::endl;//TODO удалить
    }
}

void SQL_database::create() {
    this->drop();

    std::string command = fmt::format("CREATE DATABASE {};", database_name);
    execute(*database, NANODBC_TEXT(command));

    this->use();

    command = "CREATE TABLE sites("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "site TEXT NOT NULL, "
              "is_indexed BOOLEAN NOT NULL DEFAULT false"
              ");";
    execute(*database, NANODBC_TEXT(command));

    command = "CREATE TABLE words("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "word TEXT NOT NULL, "
              "quality INT NOT NULL DEFAULT 0, "
              "site_id INT NOT NULL, "
              "FOREIGN KEY (site_id) references sites(id) "
              ");";
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::drop() {
    std::string command = fmt::format("DROP DATABASE IF EXISTS {};", database_name);
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::use() {
    std::string command = fmt::format("USE {};", database_name);
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::insert(std::string& site) {
    std::string command = fmt::format("INSERT INTO sites (site) value (\"{}\");", site);
    execute(*database, NANODBC_TEXT(command));
}

bool SQL_database::contains(std::string& site) {
    std::string command = fmt::format("SELECT * FROM sites WHERE site=\"{}\";", site);
    auto result = execute(*database, NANODBC_TEXT(command));
    return result.next();
}