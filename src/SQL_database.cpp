#include "SQL_database.h"
#include <regex>
#include <fmt/core.h>
#include <base64/include/base64.hpp>    //base64 требуется из-за проблем при добавлении текста
#include "Config.h"
#include "Exception.h"
#include "GumboAPI.h"
#include "Stemming.h"

using namespace base64;

SQL_database::SQL_database() {
    this->connection();     //подключиться к бд
}

SQL_database::~SQL_database(){
    database->disconnect();
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
    //удалить старые таблицы
    this->drop("page");
    this->drop("word");
    this->drop("search_index");

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
    try{
        execute(*database, NANODBC_TEXT(command));
    }
    catch (...) {}  //если таблицы не существует
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
        return 0;   //не найдено
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
        return 0;   //не найдено
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

nlohmann::json SQL_database::search(std::unordered_set<std::string>& worlds) {
    nlohmann::json data;
    std::map<int, float> relevance; //релевантность для конкретной страницы

    //поиск страниц на которых встречается слово
    for(auto& world : worlds) {
        std::string command = fmt::format(R"(SELECT page_id, rank FROM search_index JOIN word ON word.id = search_index.word_id WHERE value = '{}';)",
                                          to_base64(world));
        auto result = execute(*database, NANODBC_TEXT(command));
        while (result.next()){
            relevance[result.get<int>("page_id")] += result.get<float>("rank");
        }
    }

    if(relevance.empty()) return {};    //слова не найдены в бд

    //страница с максимальной релевантностью
    auto max_relevance = std::max_element(relevance.begin(), relevance.end(), [](std::pair<int, float> first, std::pair<int, float> second){
        return first.second < second.second;
    });

    Stemming stemming;
    //добавление значений в json
    for(auto& page_relevance : relevance){
        auto position = data.size();

        std::string command = fmt::format("SELECT content, path FROM page WHERE id={};", page_relevance.first);
        auto result = execute(*database, NANODBC_TEXT(command));
        result.next();
        GumboAPI html_parse(from_base64(result.get<std::string>("content")));

        html_parse.get_fragments([&](std::string fragment){      //получение фрагмента содержащего редкое слово
            std::stringstream text(fragment);
            while(!text.eof()){                //парсинг фрагмент
                std::string word;
                text >> word;
                if(worlds.count(stemming.word_stemming(word))){     //если найденное слово находится среди слов, заданных в поиске
                    fragment.insert(fragment.find(word) + word.size(), "</b>"); //вставка </b> в конец слова во фрагменте
                    fragment.insert(fragment.find(word), "<b>");    //вставка <b> в начало слова во фрагменте
                    data[position]["snippet"] = fragment;
                    return; //выход их лямбда функции
                }
            }
        });
        data[position]["title"] = html_parse.find_title();
        data[position]["uri"] = from_base64(result.get<std::string>("path"));
        data[position]["relevance"] = page_relevance.second / max_relevance->second;
    }

    //сортировка json по значению relevance
    std::sort(data.begin(), data.end(), [](nlohmann::json& first, nlohmann::json& second){
        return first["relevance"] > second["relevance"];
    });
    return data;
}