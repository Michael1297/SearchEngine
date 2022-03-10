#ifndef SEARCHENGINE_CONFIG_H
#define SEARCHENGINE_CONFIG_H

#pragma once
#include <string>
#include <mINI/src/mini/ini.h>
#include "Exception.h"

class Config{
public:
    Config();
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

    server = config["DEFAULT"]["server"];
    if(server.empty()) throw InvalidConfigFileException();

    port = config["DEFAULT"]["port"];
    if(port.empty()) throw InvalidConfigFileException();

    database = config["DEFAULT"]["database"];
    if(database.empty()) throw InvalidConfigFileException();

    login = config["DEFAULT"]["login"];
    if(login.empty()) throw InvalidConfigFileException();

    password = config["DEFAULT"]["password"];
    if(password.empty()) throw InvalidConfigFileException();
}

#endif //SEARCHENGINE_CONFIG_H
