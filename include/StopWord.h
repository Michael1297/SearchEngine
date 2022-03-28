#pragma once
#include <unordered_set>
#include <string>

class StopWord{
    std::unordered_set<std::string> words;
public:
    StopWord();
    bool contains(std::string& word);
};