#include <iostream>
#include "nlohmann/json.hpp"
#include "SearchEngine.h"
#include "Stemming.h"
#include "SQL_database.h"

int main() {
    //for(auto i : nanodbc::list_drivers()) std::cout << i.name << "\n";
    SQL_database database;

    //Stemming stemming;
    //std::cout << stemming.word_stemming("документация") << "\n";
    //std::cout << stemming.word_stemming("documentation") << "\n";

    //Config config;
    //std::cout << config.name << "\n";
    //for(auto i : nanodbc::list_drivers()) std::cout << i.name << "\n";

    //SearchEngine searchEngine("http://www.videocardz.com/");
    //searchEngine.startIndexing();


}