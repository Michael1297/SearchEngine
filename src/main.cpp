#include <exception>
#include "SearchEngine.h"
#include "Server.hpp"

int main() {
    std::unique_ptr<SearchEngine> searchEngine;
    try{
        searchEngine = std::make_unique<SearchEngine>();
    }
    catch (std::exception& x) {
        std::cerr << x.what() << "\n";
        return 0;
    }

    /* Init oatpp Environment */
    oatpp::base::Environment::init();

    /* Run App */
    run_server(searchEngine);

    /* Destroy oatpp Environment */
    oatpp::base::Environment::destroy();

}