#include "Stemming.h"

std::string Stemming::WStringToString(const std::wstring& Wstring) {
    return converter.to_bytes(Wstring);
}

std::wstring Stemming::StringToWString(const std::string& string) {
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

std::string Stemming::word_stemming(const std::string& input_word) {
    std::wstring word = this->StringToWString(input_word);   //преобразование string в wstring
    if(std::regex_search(input_word, ru_word_regex)){        //слово содержит русские буквы
        this->ru_to_lower(word);                                   //буквы в нижний регистр
        StemRussian(word);
    } else{
        this->eng_to_lower(word);                                 //буквы в нижний регистр
        StemEnglish(word);
    }
    return this->WStringToString(word);                    //преобразование wstring в string и возврат значения
}