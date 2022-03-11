#include <iostream>
#include "nlohmann/json.hpp"
#include "SearchEngine.h"
#include "Stemming.h"


int main() {
    //for(auto i : nanodbc::list_drivers()) std::cout << i.name << "\n";

    //Stemming stemming;
    //std::cout << stemming.word_stemming("документация") << "\n";
    //std::cout << stemming.word_stemming("documentation") << "\n";

    //for(auto i : nanodbc::list_drivers()) std::cout << i.name << "\n";

    SearchEngine searchEngine("http://www.videocardz.com/");
    searchEngine.startIndexing();


}