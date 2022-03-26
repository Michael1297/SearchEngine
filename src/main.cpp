#include "SearchEngine.h"
#include "Server.hpp"

int main() {
    SearchEngine searchEngine;
    //https://oatpp.io/docs/start/step-by-step/
    /* Init oatpp Environment */
    oatpp::base::Environment::init();

    /* Run App */
    run_server(searchEngine);

    /* Destroy oatpp Environment */
    oatpp::base::Environment::destroy();
}