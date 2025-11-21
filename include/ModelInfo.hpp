#ifndef MODELINFO
#define MODELINFO

#include <map>
#include <string>
#include <vector>
#include <ostream>

enum LetterOutcome {
    Match,
    Complete,
    Fallback,
    NoMatch,
    Untracked
};

struct ListInfo{
    int id;
    int currentLocation;
    int guessStreamSize;
    int promptRecord;
    std::vector<char> packetStream;
    std::vector<LetterOutcome> packetCorrectness;
    std::vector<char> packetCorrespondingQuery;
    std::vector<int> packetBestGuessLocation;

    public:
        bool operator==(const ListInfo &other) const;
        ListInfo();
        ListInfo(int id, int currentLocation,int guessStreamSize, int promptRecord, std::vector<char> &packetStream, 
            std::vector<LetterOutcome> &packetCorrectness, std::vector<char> &packetCorrespondingQuery, std::vector<int> &packetBestGuessLocation);
};

std::ostream& operator<<(std::ostream &os, const ListInfo add);

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