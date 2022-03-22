#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include "SearchEngine.h"
#include "Stemming.h"
#include "Config.h"
#include "Server.hpp"


int main() {
    auto config = std::make_shared<Config>();
    auto searchEngine = std::make_shared<SearchEngine>(config);
    //https://oatpp.io/docs/start/step-by-step/
    /* Init oatpp Environment */
    oatpp::base::Environment::init();

    /* Run App */
    run_server(config, searchEngine);

    /* Destroy oatpp Environment */
    oatpp::base::Environment::destroy();

    //for(auto i : nanodbc::list_drivers()) std::cout << i.name << "\n";

    //Stemming stemming;
    //std::cout << stemming.word_stemming("документация") << "\n";
    //std::cout << stemming.word_stemming("documentation") << "\n";

    //for(auto i : nanodbc::list_drivers()) std::cout << i.name << "\n";

    //SearchEngine searchEngine("http://www.videocardz.com/");
    //searchEngine.startIndexing();


}