#pragma once
#include <string>

class Config{
public:
    Config(const Config& root) = delete;
    Config& operator=(const Config&) = delete;
    static Config& Instance();
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
