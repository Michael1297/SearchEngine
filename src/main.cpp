#include <iostream>
#include "nlohmann/json.hpp"
#include "SearchEngine.h"
#include <nanodbc/nanodbc.h>
#include "Config.h"
#include "Stemming.h"

int main() {
    //Stemming stemming;
    //std::cout << stemming.word_stemming("документация") << "\n";
    //std::cout << stemming.word_stemming("documentation") << "\n";

    //Config config;
    //std::cout << config.name << "\n";
    //for(auto i : nanodbc::list_drivers()) std::cout << i.name << "\n";

    //std::string test = "тест";
    //std::cout << test << "\n";

    SearchEngine searchEngine("http://www.videocardz.com/");
    searchEngine.startIndexing();

    /*try
    {
        // Write your code here and have fun with nanodbc!

        auto const connection_string = NANODBC_TEXT("");
        nanodbc::connection conn(connection_string);
        conn.dbms_name();
    }
    catch (std::runtime_error const& e)
    {
        std::cerr << e.what() << std::endl;
    }*/

}