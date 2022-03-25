#include "Config.h"
#include <mINI/src/mini/ini.h>
#include "Exception.h"

Config::Config() {
    mINI::INIFile config_file("config.ini");
    mINI::INIStructure config;
    if(!config_file.read(config)) throw Exception("Missing config file");

    driverSQL = config["ConfigSQL"]["driver"];
    if(driverSQL.empty()) throw Exception("Invalid config file");

    serverSQL = config["ConfigSQL"]["server"];
    if(serverSQL.empty()) throw Exception("Invalid config file");

    portSQL = config["ConfigSQL"]["port"];
    if(portSQL.empty() || portSQL.find_first_not_of("0123456789") != std::string::npos) throw Exception("Invalid config file");

    databaseSQL = config["ConfigSQL"]["database"];
    if(databaseSQL.empty()) throw Exception("Invalid config file");

    loginSQL = config["ConfigSQL"]["login"];
    if(loginSQL.empty()) throw Exception("Invalid config file");

    passwordSQL = config["ConfigSQL"]["password"];
    if(passwordSQL.empty()) throw Exception("Invalid config file");

    start_page = config["Server"]["start_page"];
    if(start_page.empty()) throw Exception("Invalid config file");

    host = config["Server"]["host"];
    if(host.empty()) throw Exception("Invalid config file");

    port = config["Server"]["port"];
    if(port.empty() || port.find_first_not_of("0123456789") != std::string::npos) throw Exception("Invalid config file");
}

Config& Config::Instance() {
        static Config theSingleInstance;
        return theSingleInstance;
}
