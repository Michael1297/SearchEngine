#include <iostream>
#include "nlohmann/json.hpp"
#include "SearchEngine.h"
#include <nanodbc/nanodbc.h>

int main() {
    SearchEngine searchEngine("http://www.videocardz.com/");
    searchEngine.startIndexing();

    /*try
    {
        // Write your code here and have fun with nanodbc!

        auto const connection_string = NANODBC_TEXT("");
        nanodbc::connection conn(connection_string);
        conn.dbms_name();
    }
    catch (std::runtime_error const& e)
    {
        std::cerr << e.what() << std::endl;
    }*/
    //cpr::Response r = cpr::Get(cpr::Url("vk.com"));
    //std::cout << r.text << "\n";
/*
    std::wstring text(L"documentation");
    stemming::english_stem<> StemEnglish;
    StemEnglish(text);
    std::wcout << text.c_str() << "\n";

    std::cout << r.text << "\n";
    //std::wstring text1(r.text.begin(), r.text.end());
    //std::wcout << text1 << "\n";
    system("pause");
*/
}