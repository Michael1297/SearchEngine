#ifndef SEARCHENGINE_CONFIG_H
#define SEARCHENGINE_CONFIG_H

#pragma once
#include <string>
#include <mINI/src/mini/ini.h>
#include "Exception.h"

class Config{
public:
    Config();
    std::string driver;
    std::string server;
    std::string port;
    std::string database;
    std::string login;
    std::string password;
};


Config::Config() {
    mINI::INIFile config_file("config.ini");
    mINI::INIStructure config;
    if(!config_file.read(config)) throw MissingConfigFileException();

    driver = config["Config"]["driver"];
    if(driver.empty()) throw InvalidConfigFileException();

    server = config["Config"]["server"];
    if(server.empty()) throw InvalidConfigFileException();

    port = config["Config"]["port"];
    if(port.empty()) throw InvalidConfigFileException();

    database = config["Config"]["database"];
    if(database.empty()) throw InvalidConfigFileException();

    login = config["Config"]["login"];
    if(login.empty()) throw InvalidConfigFileException();

    password = config["Config"]["password"];
    if(password.empty()) throw InvalidConfigFileException();
}

#endif //SEARCHENGINE_CONFIG_H
