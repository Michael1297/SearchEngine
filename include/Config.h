#pragma once
#include <string>

class Config{
public:
    Config(const Config& root) = delete;
    Config& operator=(const Config&) = delete;
    static Config& Instance();
    Config();
    std::string driverSQL;      //драйвер бд
    std::string serverSQL;      //адрес бд
    std::string portSQL;        //порт бд
    std::string databaseSQL;    //название бд
    std::string loginSQL;       //логин к бд
    std::string passwordSQL;    //пароль к бд
    std::string start_page;     //страница с которой начать индексацию
    std::string host;           //адрес сервера
    std::string port;           //порт сервера
};
