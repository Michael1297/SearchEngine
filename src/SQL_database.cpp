#include "SQL_database.h"
#include <regex>
#include <fmt/core.h>
#include <base64/include/base64.hpp>    //base64 требуется из-за проблем при добавлении текста
#include "Config.h"
#include "Exception.h"
#include "GumboAPI.h"

using namespace base64;

SQL_database::SQL_database() {
    this->connection();     //подключиться к бд
}

void SQL_database::connection() {
    //строка подключения к бд
    //  Driver={<driver name>};Server=<server>;Database=<database>;UID=<user id>;PWD=<password>
    std::string connection_string = fmt::format("Driver={};Server={};Database={};Port={};UID={};PWD={};",
                                                Config::Instance().driverSQL, Config::Instance().serverSQL,
                                                Config::Instance().databaseSQL, Config::Instance().portSQL,
                                                Config::Instance().loginSQL, Config::Instance().passwordSQL);
    try{
        database = std::make_unique<nanodbc::connection>(NANODBC_TEXT(connection_string));
    }
    catch (...){
        throw Exception("Failed to connect to database");   //не удалось подключиться к бд
    }
}

void SQL_database::create() {
    try{        //удалить старые значения
        this->drop("page");
        this->drop("word");
        this->drop("search_index");
    }
    catch (...) {}

    std::string command = "CREATE TABLE page("
                          "id SERIAL NOT NULL PRIMARY KEY, "
                          "path TEXT NOT NULL, "        //адрес страницы от корня сайта
                          "code INT NOT NULL, "         //код ответа, полученный при запросе страницы
                          "content TEXT NOT NULL"       //контент страницы (HTML-код)
                          ");";
    execute(*database, NANODBC_TEXT(command));

    command = "CREATE TABLE word("
              "id SERIAL NOT NULL PRIMARY KEY, "
              "value TEXT NOT NULL, "       //нормальная форма слова
              "frequency INT NOT NULL"      //количество страниц, на которых слово встречается хотя бы один раз
              ");";
    execute(*database, NANODBC_TEXT(command));

    command = "CREATE TABLE search_index("
              "id SERIAL NOT NULL PRIMARY KEY, "
              "page_id INT NOT NULL, "      //идентификатор страницы.
              "word_id INT NOT NULL, "      //идентификатор слова
              "rank FLOAT NOT NULL, "       //ранг слова в данном поле этой страницы
              "FOREIGN KEY (page_id) references page(id) ON DELETE CASCADE, "
              "FOREIGN KEY (word_id) references word(id) ON DELETE CASCADE"
              ");";
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::drop(std::string table) {
    std::string command = fmt::format("DROP TABLE {} CASCADE;", table);
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::insert_page(std::string path, int code, std::string content) {
    std::string command = fmt::format(R"(INSERT INTO page(path, code, content) VALUES('{}', {}, '{}');)", to_base64(path), code, to_base64(content));
    execute(*database, NANODBC_TEXT(command));
}

int SQL_database::page_id(std::string path) {
    std::string command = fmt::format(R"(SELECT id FROM page WHERE path='{}' LIMIT 1;)", to_base64(path));
    auto result = execute(*database, NANODBC_TEXT(command));
    if(result.next()) {
        return result.get<int>("id");
    } else {
        return 0;
    }
}

void SQL_database::insert_word(std::string value) {
    std::string command = fmt::format(R"(INSERT INTO word(value, frequency) VALUES ('{}', 1);)", to_base64(value));
    execute(*database, NANODBC_TEXT(command));
}

int SQL_database::word_id(std::string value) {
    std::string command = fmt::format(R"(SELECT id FROM word WHERE value='{}' LIMIT 1;)", to_base64(value));
    auto result = execute(*database, NANODBC_TEXT(command));
    if(result.next()) {
        return result.get<int>("id");
    } else {
        return 0;
    }
}

void SQL_database::update_word(std::string value) {
    std::string command = fmt::format(R"(UPDATE word SET frequency=frequency+1 WHERE value='{}';)", to_base64(value));
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::insert_search_index(int page_id, int word_id, float rank) {
    std::string command = fmt::format(R"(INSERT INTO search_index(page_id, word_id, rank) VALUES ({}, {}, {});)", page_id, word_id, rank);
    execute(*database, NANODBC_TEXT(command));
}

void SQL_database::erase_page(std::string path) {
    path = to_base64(path);
    //уменьшить значение frequency на 1
    std::string command = fmt::format("UPDATE word SET frequency=frequency-1 FROM search_index, page "
                                      "WHERE word.id=search_index.word_id AND page.id=search_index.page_id AND page.path='{}';", path);
    execute(*database, NANODBC_TEXT(command));

    //удалить word где frequency < 1
    command = "DELETE FROM word WHERE frequency < 1;";
    execute(*database, NANODBC_TEXT(command));

    //удалить страницу
    command = fmt::format("DELETE FROM page WHERE path=\'{}\';", path);
    execute(*database, NANODBC_TEXT(command));
}

int SQL_database::size(std::string table) {
    std::string command = fmt::format("SELECT count(*) from {};", table);
    auto result = execute(*database, NANODBC_TEXT(command));
    result.next();
    return result.get<int>("count");
}

void SQL_database::general(std::unordered_set<int>& main, std::unordered_set<int>& other) {
    for(auto& single : main){
        if(!other.count(single)) main.erase(single);
    }
}

float SQL_database::get_relevance(int page_id, std::string world) {
    std::string command = fmt::format("SELECT rank FROM word JOIN search_index ON word.id = search_index.word_id "
                                      "WHERE page_id={} AND value=\'{}\';", page_id, to_base64(world));
    auto result = execute(*database, NANODBC_TEXT(command));
    if(result.next()){
        return result.get<float>("rank");
    } else{
        return 0;
    }
}

nlohmann::json SQL_database::search(std::unordered_set<std::string>& worlds) {
    nlohmann::json data;
    std::pair<std::string, int> minimum = {"", 0}; //используется для хранения минимального value + frequency

    //поиск минимального value и frequency
    for(auto& world : worlds) {
        std::string command = fmt::format(R"(SELECT value, frequency FROM word WHERE value='{}';)", to_base64(world));
        auto result = execute(*database, NANODBC_TEXT(command));

        if(result.next()) {
            if(minimum.first.empty() || minimum.second > result.get<int>("frequency")){ //добавляем 1 значение или меняем если frequency меньше
                minimum.first = result.get<std::string>("value");
                minimum.second = result.get<int>("frequency");
            }
        } else{
            worlds.erase(world);    //слово не найдено в бд
        }
    }

    if(minimum.first.empty()) return {};    //слова не найдены в бд

    std::unordered_set<int> minimum_page_id;    //page_id минимального слова
    {
        std::string command = fmt::format(R"(SELECT page_id FROM word JOIN search_index ON word.id = search_index.word_id WHERE value='{}';)", minimum.first);
        auto result = execute(*database, NANODBC_TEXT(command));
        while (result.next()) minimum_page_id.insert(result.get<int>("page_id"));   //добавление page_id в массив page_id
    }

    //поиск общих ссылок всех слов
    for(auto& world : worlds){
        std::string base64_world = to_base64(world);
        if(base64_world == minimum.first) continue;
        std::unordered_set<int> others_page_id;
        std::string command = fmt::format(R"(SELECT page_id FROM word JOIN search_index ON word.id = search_index.word_id WHERE value='{}';)", base64_world);
        auto result = execute(*database, NANODBC_TEXT(command));
        while (result.next()) others_page_id.insert(result.get<int>("page_id"));   //добавление page_id в массив others_page_id
        this->general(minimum_page_id, others_page_id); //удалить из 1 массива id, не найденные во 2 массива
    }

    float total_relevance = 0;      //максимальная абсолютная релевантность
    std::map<int, float> relevance; //релевантность для конкретной страницы
    for(auto& id : minimum_page_id){    //вычисление релевантности
        float page_relevance = 0;
        for(auto& world : worlds) {
            page_relevance += this->get_relevance(id, world);
        }
        total_relevance += page_relevance;
        relevance[id] = page_relevance;
    }

    std::regex fragment_regex("(^|\\s)" + from_base64(minimum.first) + "($|\\s)", std::regex::icase);

    //добавление значений в json
    for(auto& id : minimum_page_id){
        auto count = data.size();

        std::string command = fmt::format("SELECT content, path FROM page WHERE id={};", id);
        auto result = execute(*database, NANODBC_TEXT(command));
        result.next();
        GumboAPI html_parse(from_base64(result.get<std::string>("content")));

        html_parse.get_fragments([&](std::string fragment){      //получение фрагмента содержащего редкое слово
            if(std::regex_search(fragment, fragment_regex)){
                data[count]["snippet"] = std::regex_replace(fragment, fragment_regex, " <b>" + from_base64(minimum.first) + "</b> ");
                return;
            }
        });

        data[count]["title"] = html_parse.find_title();
        data[count]["uri"] = from_base64(result.get<std::string>("path"));
        data[count]["relevance"] = relevance[id] / total_relevance;
    }

    //сортировка json по значению relevance
    std::sort(data.begin(), data.end(), [](nlohmann::json& first, nlohmann::json& second){
        return first["relevance"] > second["relevance"];
    });
    return data;
}