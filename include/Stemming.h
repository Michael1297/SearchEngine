#ifndef SEARCHENGINE_STEMMING_H
#define SEARCHENGINE_STEMMING_H

#pragma once
#include <regex>
#include <codecvt>
#include <olestem/stemming/english_stem.h>
#include <olestem/stemming/russian_stem.h>

class Stemming{
    stemming::english_stem<> StemEnglish;
    stemming::russian_stem<> StemRussian;
    std::regex ru_word_regex = std::regex("[а-я]", std::regex::icase);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
public:
    std::string WStringToString(std::wstring Wstring);      //wstring преобразовать в string
    std::wstring StringToWString(std::string string);       //string преобразовать в wstring
    void eng_to_lower(std::wstring& word);                  //eng буквы в нижний регистр
    void ru_to_lower(std::wstring& word);                   //ru буквы в нижний регистр
    std::string stemming(std::string word);                 //стимминг
};

#endif //SEARCHENGINE_STEMMING_H
