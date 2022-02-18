#pragma once
#ifndef MAIN_CPP_SEARCHENGINE_H
#define MAIN_CPP_SEARCHENGINE_H

#include <string>
#include <regex>
#include <unordered_set>
#include <olestem/stemming/english_stem.h>
#include "gumbo.h"

class SearchEngine{
    std::string link;
    std::regex not_word = std::regex("^\\W+$|^\\d+$");  //рандомные символы вместо слова
    std::regex own_link;                                   //коректныя ссылка
    stemming::english_stem<> StemEnglish;
    std::unordered_set<std::string> words;      //Todo добавлено временно
    void clean_text(GumboNode* node, std::string str = "");//парсинг слов из HTML5 кода
    void search_for_links(GumboNode* node);                //поиск ссылок в HTML5 коде
    std::string StemEnglishFunc(std::string str);          //функция стемминга слова
public:
    SearchEngine(const std::string& _link);                       //ввод ссылки + генерация own_link regex для проверки ссылок
    void startIndexing();

};

#endif //MAIN_CPP_SEARCHENGINE_H