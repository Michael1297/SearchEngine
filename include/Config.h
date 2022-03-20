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
    if(!config_file.read(config)) throw Exception("Missing config file");

    driver = config["Config"]["driver"];
    if(driver.empty()) throw Exception("Invalid config file");

    server = config["Config"]["server"];
    if(server.empty()) throw Exception("Invalid config file");

    port = config["Config"]["port"];
    if(port.empty()) throw Exception("Invalid config file");

    database = config["Config"]["database"];
    if(database.empty()) throw Exception("Invalid config file");

    login = config["Config"]["login"];
    if(login.empty()) throw Exception("Invalid config file");

    password = config["Config"]["password"];
    if(password.empty()) throw Exception("Invalid config file");
}

#endif //SEARCHENGINE_CONFIG_H
