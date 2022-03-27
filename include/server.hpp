#pragma once
#include <functional>
#include "Config.h"
#include "SearchEngine.h"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

class Handler : public oatpp::web::server::HttpRequestHandler {
    std::function<std::string(const std::shared_ptr<IncomingRequest>& request)> function_out;
public:
    Handler(std::function<std::string(const std::shared_ptr<IncomingRequest>& request)> function): function_out(function){}
    /**
     * Handle incoming request and return outgoing response.
     */
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        return ResponseFactory::createResponse(Status::CODE_200, function_out(request));
    }
};

//https://oatpp.io/docs/start/step-by-step/
void run_server(std::unique_ptr<SearchEngine>& searchEngine) {

    /* Create Router for HTTP requests routing */
    auto router = oatpp::web::server::HttpRouter::createShared();


    /** Route GET - "/startIndexing" requests to Handler */
    router->route("GET", "/startIndexing", std::make_shared<Handler>([&searchEngine](const std::shared_ptr<Handler::IncomingRequest>& request){
        std::string tail = request->getPathTail();      /* get url 'tail' - everything that comes after '*' */
        auto queryParams = oatpp::network::Url::Parser::parseQueryParams(tail);     /* parse query params from tail */
        std::string queurls = queryParams.get("queurls").getValue("");
        if(queurls.empty()){
            return searchEngine->startIndexing().dump(1, '\t');
        } else{
            return searchEngine->startIndexing(queurls).dump(1, '\t');
        }

    }));

    /** Route GET - "/stopIndexing" requests to Handler */
    router->route("GET", "/stopIndexing", std::make_shared<Handler>([&searchEngine](const std::shared_ptr<Handler::IncomingRequest>& request){
        return searchEngine->stopIndexing().dump(1, '\t');
    }));

    /** Route GET - "/status" requests to Handler */
    router->route("GET", "/status", std::make_shared<Handler>([&searchEngine](const std::shared_ptr<Handler::IncomingRequest>& request){
        return searchEngine->status().dump(1, '\t');
    }));

    /** Route GET - "/search" requests to Handler */
    router->route("GET", "/search", std::make_shared<Handler>([&searchEngine](const std::shared_ptr<Handler::IncomingRequest>& request){
        std::string tail = request->getPathTail();      /* get url 'tail' - everything that comes after '*' */
        auto queryParams = oatpp::network::Url::Parser::parseQueryParams(tail);     /* parse query params from tail */
        std::string query = queryParams.get("query").getValue("");
        std::string offset = queryParams.get("offset").getValue("0");
        std::string limit = queryParams.get("limit").getValue("2147483647");
        return searchEngine->search(query, std::stoi(offset), std::stoi(limit)).dump(1, '\t');
    }));


    /* Create HTTP connection handler with router */
    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    /* Create TCP connection provider */
    auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({Config::Instance().host,
                                                                                             static_cast<v_uint16>(std::stoi(Config::Instance().port)), oatpp::network::Address::IP_4});

    /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
    oatpp::network::Server server(connectionProvider, connectionHandler);

    /* Print info about server port */  //TODO del
    OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

    /* Run server */
    server.run();
}
