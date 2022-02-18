#include <iostream>
#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "gumbo.h"
#include "olestem/stemming/english_stem.h"
#include "SearchEngine.h"

int main() {
    SearchEngine searchEngine("videocardz.com/");
    searchEngine.startIndexing();
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