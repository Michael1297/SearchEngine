#include "Stemming.h"
#include <clocale>
#include <windows.h>

std::string Stemming::WStringToString(std::wstring Wstring) {
    return converter.to_bytes(Wstring);
}

std::wstring Stemming::StringToWString(std::string string) {
    return converter.from_bytes(string);
}

void Stemming::eng_to_lower(std::wstring& word) {
    for(auto& i : word){
        if (i >= L'A' && i <= L'Z' ) i += L'a' - L'A';  //англиские буквы
    }
}

void Stemming::ru_to_lower(std::wstring& word) {
    for(auto& i : word){
        if (i >= L'А' && i <= L'Я' ) i += L'а' - L'А';  //русские буквы
    }
}

std::string Stemming::stemming(std::string word) {
    std::wstring temp = this->StringToWString(word);        //преобразование string в wstring
    if(std::regex_search(word, ru_word_regex)){             //слово содержит русские буквы
        this->ru_to_lower(temp);                              //буквы в нижний регистр
        StemRussian(temp);
    } else{
        this->eng_to_lower(temp);                             //буквы в нижний регистр
        StemEnglish(temp);
    }
    return this->WStringToString(temp);                    //преобразование wstring в string и возврат значения
}