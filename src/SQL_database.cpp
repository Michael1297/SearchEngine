#include "SQL_database.h"
#include <sstream>
#include "Config.h"
#include <iostream> //TODO удалить

SQL_database::SQL_database() {
    //  https://docs.microsoft.com/en-us/sql/integration-services/import-export-data/connect-to-a-postgresql-data-source-sql-server-import-and-export-wizard?view=sql-server-2017
    //  https://www.connectionstrings.com/postgresql-odbc-driver-psqlodbc/
    //  Driver={PostgreSQL ODBC Driver(UNICODE)};Server=<server>;Port=<port>;Database=<database>;UID=<user id>;PWD=<password>
    //  Driver={PostgreSQL ODBC Driver(ANSI)};Server=<server>;Port=<port>;Database=<database>;UID=<user id>;PWD=<password>
    Config config;
    std::stringstream connection_string;
    connection_string
    << "Driver={PostgreSQL ODBC Driver(ANSI)};"
    << "Server=" << config.server << ";"
    << "Port=" << config.port
    << "Database=" << config.database << ";"
    << "UID=" << config.login << ";"
    << "PWD=" << config.password << ";";

    try{
        conn = std::make_unique<nanodbc::connection>(NANODBC_TEXT(connection_string.str()));
        std::cout << conn->dbms_name() << "\n";
    }
    catch (std::runtime_error const& e){
        std::cerr << e.what() << std::endl;
    }

}