#ifndef MODELINFO
#define MODELINFO

#include "MonkeyTyper.hpp"

#include <map>
#include <string>

struct TyperInfo{
    ListInfo listInfo;
    std::string stream;
    std::string prompt;
    unsigned int seed;

    TyperInfo(ListInfo& listInfo, std::string stream, std::string prompt, unsigned int seed);
};

struct ModelInfo{
    std::map<int,std::string> messages;
    std::map<int,TyperInfo> typerValues;

    ModelInfo(std::map<int,std::string> &messages,std::map<int,TyperInfo> &typerValues);
};

#endif