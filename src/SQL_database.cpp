#include "SQL_database.h"
#include <fmt/core.h>
#include <base64/include/base64.hpp>
#include "Config.h"
#include "Exception.h"

SQL_database::SQL_database(std::shared_ptr<Config> config) {
    database_name = config->databaseSQL;

    //строка подключения к бд
    //  Driver={MySQL ODBC 8.0 ANSI Driver};Server=<server>;Database=<database>;UID=<user id>;PWD=<password>
    std::string connection_string = fmt::format("Driver={};Server={};Port={};UID={};PWD={};",
                                                config->driverSQL, config->serverSQL, config->portSQL, config->loginSQL, config->passwordSQL);
    try{
        database = std::make_unique<nanodbc::connection>(NANODBC_TEXT(connection_string));
    }
    catch (...){
        throw Exception("Failed to connect to database");   //не удалось подключиться к бд
    }
    this->use();    //использовать бд
}

void SQL_database::create() {
    this->drop();

    std::string command = fmt::format("CREATE DATABASE {};", database_name);
    execute(*database, NANODBC_TEXT(command));

    this->use();    //использовать бд

    command = "CREATE TABLE page("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "path TEXT NOT NULL, "        //адрес страницы от корня сайта
              "code INT NOT NULL, "         //код ответа, полученный при запросе страницы
              "content MEDIUMTEXT NOT NULL" //контент страницы (HTML-код)   (MEDIUMTEXT - в TEXT все не помещается)
              ");";
    execute(*database, NANODBC_TEXT(command));

    command = "CREATE TABLE word("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              //"site_id INT NOT NULL, "    //идентификатор сайта (что это?)
              "value TEXT NOT NULL, "       //нормальная форма слова
              "frequency INT NOT NULL"      //количество страниц, на которых слово встречается хотя бы один раз
              ");";
    execute(*database, NANODBC_TEXT(command));

    command = "CREATE TABLE search_index("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "page_id INT NOT NULL, "      //идентификатор страницы.
              "word_id INT NOT NULL, "      //идентификатор слова
              "rnk FLOAT NOT NULL, "         //ранг слова в данном поле этой страницы
              "FOREIGN KEY (page_id) references page(id), "
              "FOREIGN KEY (word_id) references word(id) "
              ");";
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::drop() {
    std::string command = fmt::format("DROP DATABASE IF EXISTS {};", database_name);
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::use() {
    std::string command = fmt::format("USE {};", database_name);
    try{
        execute(*database, NANODBC_TEXT(command));  //использовать бд
    }
    catch (...){    //не удалось использовать бд
        this->create();    //создать новую бд
    }

}

void SQL_database::insert_page(std::string path, int code, std::string content) {
    //base64 требуется из-за проблем при добавлении кода страницы
    std::string command = fmt::format(R"(INSERT INTO page(path, code, content) VALUES("{}", {}, "{}");)", path, code, base64::to_base64(content));
    execute(*database, NANODBC_TEXT(command));
}

int SQL_database::page_id(std::string path) {
    std::string command = fmt::format(R"(SELECT id FROM page WHERE path="{}";)", path);
    auto result = execute(*database, NANODBC_TEXT(command));
    if(result.next()) {
        return result.get<int>("id");
    } else {
        return 0;
    }
}

void SQL_database::insert_word(std::string value) {
    std::string command = fmt::format(R"(INSERT INTO word(value, frequency) VALUES ("{}", 1);)", value);
    execute(*database, NANODBC_TEXT(command));
}

int SQL_database::word_id(std::string value) {
    std::string command = fmt::format(R"(SELECT id FROM word WHERE value="{}";)", value);
    auto result = execute(*database, NANODBC_TEXT(command));
    if(result.next()) {
        return result.get<int>("id");
    } else {
        return 0;
    }
}

void SQL_database::update_word(std::string value) {
    std::string command = fmt::format(R"(UPDATE word SET frequency=frequency+1 WHERE value="{}";)", value);
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::insert_search_index(int page_id, int word_id, float rank) {
    std::string command = fmt::format(R"(INSERT INTO search_index(page_id, word_id, rnk) VALUES ({}, {}, {});)", page_id, word_id, rank);
    execute(*database, NANODBC_TEXT(command));
}

int SQL_database::size(std::string table) {
    std::string command = fmt::format("SELECT id FROM {} ORDER BY id DESC LIMIT 1;", table);
    auto result = execute(*database, NANODBC_TEXT(command));
    if(result.next()) {
        return result.get<int>("id");
    } else {
        return 0;
    }
}

#include <iostream>
nlohmann::json SQL_database::search(std::unordered_set<std::string>& worlds) {
    nlohmann::json data;
    std::pair<std::string, int> minimum = {"", 0}; //используется для хранения минимального value + frequency

    //поиск минимального value и frequency
    for(auto& world : worlds) {
        std::string command = fmt::format(R"(SELECT frequency FROM word WHERE value="{}";)", world);
        nanodbc::result result;
        try{
            result = execute(*database, NANODBC_TEXT(command));
        }
        catch (...){    //возникли проблемы при поиске слова
            worlds.erase(world);
            continue;
        }
        if(result.next()) {
            if(minimum.first.empty() || minimum.second > result.get<int>("frequency")){ //добавляем 1 значение или меняем если frequency меньше
                minimum.first = world;
                minimum.second = result.get<int>("frequency");
            }
        } else{
            worlds.erase(world);    //слово не найдено в бд
        }
    }

    if(minimum.first.empty()) return {};    //слова не найдены в бд

    std::unordered_set<int> page_id;    //page_id минимального слова
    {
        std::string command = fmt::format(R"(SELECT page_id FROM word JOIN search_index ON word.id = search_index.word_id WHERE value="{}";)", minimum.first);
        auto result = execute(*database, NANODBC_TEXT(command));
        while (result.next()) page_id.insert(result.get<int>("page_id"));   //добавление page_id в массив page_id
    }

    //поиск ссылок принадлежащих другим словам
    for(auto& world : worlds){
        if(world == minimum.first) continue;
        std::unordered_set<int> others_page_id;
        std::string command = fmt::format(R"(SELECT page_id FROM word JOIN search_index ON word.id = search_index.word_id WHERE value="{}";)", world);
        auto result = execute(*database, NANODBC_TEXT(command));
        while (result.next()) others_page_id.insert(result.get<int>("page_id"));   //добавление page_id в массив others_page_id
        for(auto& id : page_id){
            if(others_page_id.count(id) == 0) page_id.erase(id); //удалить id 1 слова, так как оно не найдено во 2 слове
        }
    }

    //добавление значений в json
    for(auto& id : page_id){
        auto count = data.size();
        std::string command = fmt::format("SELECT * FROM page  WHERE id={};", id);
        auto result = execute(*database, NANODBC_TEXT(command));
        result.next();
        data[count]["uri"] = result.get<std::string>("path");

    }
    return data;
}