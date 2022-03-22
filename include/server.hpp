#pragma once
#ifndef MAIN_CPP_SERVER_HPP
#define MAIN_CPP_SERVER_HPP

#include <iostream>
#include <functional>
#include "Config.h"
#include "SearchEngine.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

class Handler : public oatpp::web::server::HttpRequestHandler {
    std::function<std::string()> function_out;
public:
    Handler(std::function<std::string()> function): function_out(function){}
    /**
     * Handle incoming request and return outgoing response.
     */
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        //std::cout << request.get()->getPathTail().getValue("ghg") << "\n";
        return ResponseFactory::createResponse(Status::CODE_200, function_out());
    }
};

//https://oatpp.io/docs/start/step-by-step/
void run_server(std::shared_ptr<Config> config, std::shared_ptr<SearchEngine> searchEngine) {

    /* Create Router for HTTP requests routing */
    auto router = oatpp::web::server::HttpRouter::createShared();

    /* Route GET - "/hello" requests to Handler */
    router->route("GET", "/hello", std::make_shared<Handler>([](){
        std::cout << "/hello\n";
        return "/hello";
    }));

    router->route("GET", "/test", std::make_shared<Handler>([](){
        std::cout << "/test\n";
        return "/test";
    }));

    router->route("GET", "/startIndexing", std::make_shared<Handler>([&searchEngine](){
        searchEngine->startIndexing();
        std::cout << "/startIndexing\n";
        return "";
    }));

    /* Create HTTP connection handler with router */
    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    /* Create TCP connection provider */
    auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({config->host, static_cast<v_uint16>(std::stoi(config->port)), oatpp::network::Address::IP_4});

    /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
    oatpp::network::Server server(connectionProvider, connectionHandler);

    /* Print info about server port */
    OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

    /* Run server */
    server.run();
}

#endif //MAIN_CPP_SERVER_HPP
