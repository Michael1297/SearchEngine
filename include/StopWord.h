#pragma once
#include <unordered_set>
#include <string>

class StopWord{
    std::unordered_set<std::string> words;  //список стоп слов
public:
    StopWord();
    bool contains(std::string& word);       //проверка наличия стоп слова в массиве
};