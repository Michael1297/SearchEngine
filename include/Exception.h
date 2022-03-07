#ifndef SEARCHENGINE_EXCEPTION_H
#define SEARCHENGINE_EXCEPTION_H

#pragma once
#include <exception>

class InvalidConfigFileException: public std::exception{
    const char* what() const noexcept override{
        return "Invalid config file";
    }
};

class MissingConfigFileException: public std::exception{
    const char* what() const noexcept override{
        return "Missing config file";
    }
};

class InvalidLinkException: public std::exception{
    const char* what() const noexcept override{
        return "Invalid link";
    }
};

#endif //SEARCHENGINE_EXCEPTION_H
