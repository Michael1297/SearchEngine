#include <iostream>
#include <memory>
#include "SearchEngine.h"
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
}