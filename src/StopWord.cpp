#include "StopWord.h"
#include <fstream>

StopWord::StopWord() {
    std::ifstream file("StopWordList.txt");
    if(!file.is_open()) return;
    while (!file.eof()){
        std::string word;
        file >> word;
        if(!word.empty()) words.insert(word);
    }
}

bool StopWord::contains(std::string& word) {
    return words.count(word);
}