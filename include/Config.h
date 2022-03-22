#ifndef SEARCHENGINE_CONFIG_H
#define SEARCHENGINE_CONFIG_H

#pragma once
#include <string>

class Config{
public:
    Config();
    std::string driverSQL;
    std::string serverSQL;
    std::string portSQL;
    std::string databaseSQL;
    std::string loginSQL;
    std::string passwordSQL;
    std::string start_page;
    std::string host;
    std::string port;
};

#endif //SEARCHENGINE_CONFIG_H
