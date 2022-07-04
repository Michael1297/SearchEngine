#include "SQL_database.h"
#include <unordered_set>
#include <fmt/core.h>
#include <ThreadPool/ThreadPool.h>
#include <base64/include/base64.hpp>    //base64 требуется из-за проблем при добавлении текста
#include "Config.h"
#include "Exception.h"
#include "GumboAPI.h"
#include "Stemming.h"

using namespace base64;

SQL_database::SQL_database() {   //подключиться к бд
    //строка подключения к бд
    //  Driver={<driver name>};Server=<server>;Database=<database>;UID=<user id>;PWD=<password>
    std::string connection_string = fmt::format("Driver={};Server={};Database={};Port={};UID={};PWD={};",
                                                Config::Instance().driverSQL, Config::Instance().serverSQL,
                                                Config::Instance().databaseSQL, Config::Instance().portSQL,
                                                Config::Instance().loginSQL, Config::Instance().passwordSQL);
    try{
        database = std::make_unique<nanodbc::connection>(connection_string);
    }
    catch (...){
        throw Exception("Failed to connect to database");   //не удалось подключиться к бд
    }

    //проверка на наличие таблиц в бд
    std::unordered_set<std::string> tables;
    std::string command = "SELECT table_name FROM information_schema.tables WHERE table_schema=\'public\';";
    auto result = execute(*database, command);
    while (result.next()){
        tables.insert(result.get<std::string>("table_name"));
    }
    //если таблицы не существуют, то создать заново
    if(tables.size() != 3 || !tables.count("page") || !tables.count("word") || !tables.count("search_index")) {
        for(auto& table : tables) this->drop(table);
        this->create();
    }
}

SQL_database::~SQL_database(){
    database->disconnect();
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
    execute(*database, command);

    command = "CREATE TABLE word("
              "id SERIAL NOT NULL PRIMARY KEY, "
              "value TEXT NOT NULL, "       //нормальная форма слова
              "frequency INT NOT NULL"      //количество страниц, на которых слово встречается хотя бы один раз
              ");";
    execute(*database, command);

    command = "CREATE TABLE search_index("
              "id SERIAL NOT NULL PRIMARY KEY, "
              "page_id INT NOT NULL, "      //идентификатор страницы.
              "word_id INT NOT NULL, "      //идентификатор слова
              "rank FLOAT NOT NULL, "       //ранг слова в данном поле этой страницы
              "FOREIGN KEY (page_id) references page(id) ON DELETE CASCADE, "
              "FOREIGN KEY (word_id) references word(id) ON DELETE CASCADE"
              ");";
    execute(*database, command);
}

void SQL_database::drop(const std::string& table) {
    std::string command = fmt::format("DROP TABLE {} CASCADE;", table);
    try{
        execute(*database, command);
    }
    catch (...) {}  //если таблицы не существует
}

void SQL_database::insert_page(const std::string& path, int code, const std::string& content) {
    std::string command = fmt::format(R"(INSERT INTO page(path, code, content) VALUES('{}', {}, '{}');)", to_base64(path), code, to_base64(content));
    execute(*database, command);
}

int SQL_database::page_id(const std::string& path) {
    std::string command = fmt::format(R"(SELECT id FROM page WHERE path='{}' LIMIT 1;)", to_base64(path));
    auto result = execute(*database, command);
    if(result.next()) {
        return result.get<int>("id");
    } else {
        return 0;   //не найдено
    }
}

void SQL_database::insert_word(const std::string& value) {
    std::string command = fmt::format(R"(INSERT INTO word(value, frequency) VALUES ('{}', 1);)", to_base64(value));
    execute(*database, command);
}

int SQL_database::word_id(const std::string& value) {
    std::string command = fmt::format(R"(SELECT id FROM word WHERE value='{}' LIMIT 1;)", to_base64(value));
    auto result = execute(*database, command);
    if(result.next()) {
        return result.get<int>("id");
    } else {
        return 0;   //не найдено
    }
}

void SQL_database::update_word(const std::string& value) {
    std::string command = fmt::format(R"(UPDATE word SET frequency=frequency+1 WHERE value='{}';)", to_base64(value));
    execute(*database, command);
}

void SQL_database::insert_search_index(int page_id, int word_id, float rank) {
    std::string command = fmt::format(R"(INSERT INTO search_index(page_id, word_id, rank) VALUES ({}, {}, {});)", page_id, word_id, rank);
    execute(*database, command);
}

void SQL_database::erase_page(std::string path) {
    path = to_base64(path);
    //уменьшить значение frequency на 1
    std::string command = fmt::format("UPDATE word SET frequency=frequency-1 FROM search_index, page "
                                      "WHERE word.id=search_index.word_id AND page.id=search_index.page_id AND page.path='{}';", path);
    execute(*database, command);

    //удалить word где frequency < 1
    command = "DELETE FROM word WHERE frequency < 1;";
    execute(*database, command);

    //удалить страницу
    command = fmt::format("DELETE FROM page WHERE path=\'{}\';", path);
    execute(*database, command);
}

int SQL_database::size(const std::string& table) {
    std::string command = fmt::format("SELECT count(*) from {};", table);
    auto result = execute(*database, command);
    result.next();
    return result.get<int>("count");
}

[[maybe_unused]] std::string SQL_database::get_page_content(const std::string& path) {
    std::string command = fmt::format("SELECT content FROM page WHERE path=\'{}\';", to_base64(path));
    auto result = execute(*database, command);
    if(result.next()){
        return from_base64(result.get<std::string>("content"));
    } else{
        return "";
    }
}

nlohmann::json SQL_database::search(std::unordered_set<std::string>& worlds) {
    nlohmann::json data;
    std::map<int, float> relevance; //релевантность для конкретной страницы

    //поиск страниц на которых встречается слово
    for(auto& world : worlds) {
        std::string command = fmt::format(R"(SELECT page_id, rank FROM search_index JOIN word ON word.id = search_index.word_id WHERE value = '{}';)",
                                          to_base64(world));
        auto result = execute(*database, command);
        while (result.next()){
            relevance[result.get<int>("page_id")] += result.get<float>("rank");
        }
    }

    if(relevance.empty()) return {};    //слова не найдены в бд

    //страница с максимальной релевантностью
    auto max_relevance = std::max_element(relevance.begin(), relevance.end(), [](std::pair<const int, float>& first, std::pair<const int, float>& second){
        return first.second < second.second;
    });

    auto* thread_pool = new ThreadPool(std::thread::hardware_concurrency());
    std::mutex mutex;
    Stemming stemming;

    //добавление значений в json
    for(auto& page_relevance : relevance){
        thread_pool->enqueue([&, page_relevance](){
            nlohmann::json search_result;
            std::string command = fmt::format("SELECT content, path FROM page WHERE id={};", page_relevance.first);
            auto result = execute(*database, command);
            result.next();
            GumboAPI html_parse(from_base64(result.get<std::string>("content")));

            //получение фрагмента содержащего редкое слово
            html_parse.get_fragments([&](const std::string& fragment){
                std::stringstream current_fragment(fragment);   //фрагмент используемый для парсинга текста
                std::stringstream new_fragment;     //буффер для хранения результата
                bool is_find = false;   //фрагмент найден
                bool is_empty = true;   //если new_fragment еще пустой - (еще нет первого слова)

                while(!current_fragment.eof()){                //парсинг фрагмент
                    std::string word;
                    current_fragment >> word;
                    if(worlds.count(stemming.word_stemming(word))){     //если найденное слово находится среди слов, заданных в поиске
                        new_fragment << (is_empty ? "" : " ") << "<b>" << word << "</b>";
                        is_find = true;
                    } else{
                        new_fragment << (is_empty ? "" : " ") << word;
                    }
                    is_empty = false;
                }
                if(is_find) {
                    search_result["snippet"] = new_fragment.str();
                    return;
                }
            });

            search_result["title"] = html_parse.find_title();
            search_result["uri"] = from_base64(result.get<std::string>("path"));
            search_result["relevance"] = page_relevance.second / max_relevance->second;
            mutex.lock();
            data.push_back(search_result);
            mutex.unlock();
        });
    }

    delete thread_pool;
    //сортировка json по значению relevance
    std::sort(data.begin(), data.end(), [](nlohmann::json& first, nlohmann::json& second){
        return first["relevance"] > second["relevance"]
            || first["relevance"] == second["relevance"] && first["uri"] < second["uri"];
    });
    return data;
}