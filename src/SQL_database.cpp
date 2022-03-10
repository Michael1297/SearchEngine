#include "SQL_database.h"
#include <sstream>
#include "Config.h"
#include <iostream> //TODO удалить

SQL_database::SQL_database() {
    //  https://docs.microsoft.com/en-us/sql/integration-services/import-export-data/connect-to-a-mysql-data-source-sql-server-import-and-export-wizard?view=sql-server-2017
    //  Driver={MySQL ODBC 5.3 ANSI Driver};Server=<server>;Database=<database>;UID=<user id>;PWD=<password>
    Config config;
    std::stringstream connection_string;
    connection_string
    << "Driver={MySQL ODBC 8.0 ANSI Driver};"
    << "Server=" << config.server << ";"
    << "Port=" << config.port << ";"
    //<< "Database=" << config.database << ";"
    << "UID=" << config.login << ";"
    << "PWD=" << config.password << ";";

    try{
        connection = std::make_unique<nanodbc::connection>(NANODBC_TEXT(connection_string.str()));
        std::cout << connection->dbms_name() << "\n";
    }
    catch (std::runtime_error const& e){
        std::cerr << e.what() << std::endl;
    }

}