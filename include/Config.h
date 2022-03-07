#ifndef SEARCHENGINE_CONFIG_H
#define SEARCHENGINE_CONFIG_H

#pragma once
#include <string>
#include <mINI/src/mini/ini.h>
#include "Exception.h"

class Config{
public:
    Config();
    std::string host;
    std::string name;
    std::string login;
    std::string password;
    std::string port;
};


Config::Config() {
    mINI::INIFile config_file("config.ini");
    mINI::INIStructure config;
    if(!config_file.read(config)) throw MissingConfigFileException();

    host = config["DEFAULT"]["host"];
    if(host.empty()) throw InvalidConfigFileException();

    name = config["DEFAULT"]["name"];
    if(name.empty()) throw InvalidConfigFileException();

    login = config["DEFAULT"]["login"];
    if(login.empty()) throw InvalidConfigFileException();

    password = config["DEFAULT"]["password"];
    if(password.empty()) throw InvalidConfigFileException();

    port = config["DEFAULT"]["port"];
    if(port.empty()) throw InvalidConfigFileException();
}

#endif //SEARCHENGINE_CONFIG_H
