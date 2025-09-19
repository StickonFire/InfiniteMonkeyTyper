#ifndef MONKEY_TYPER
#define MONKEY_TYPER

#include <string>
#include <vector>
#include <queue>
#include <random>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
using namespace std;

extern std::string default_alphabet;

struct TypedChar{
    char letter;
    int position;

    public:
        bool operator==(const TypedChar &other) const;
};

enum Status {
    Paused,
    Killed,
    Completed,
    PacketReady
};

enum LetterOutcome {
    Match,
    Fallback,
    NoMatch
};

struct ListInfo{
    int id;
    int currentLocation;
    int guessStreamSize;
    int promptRecord;
    vector<char> packetStream;
    vector<LetterOutcome> packetCorrectness;
    vector<char> packetCorrespondingQuery;
    vector<int> packetBestGuessLocation;

    public:
        ListInfo(int id, int currentLocation,int guessStreamSize, int promptRecord, vector<char> &packetStream, 
            vector<LetterOutcome> &packetCorrectness, vector<char> &packetCorrespondingQuery, vector<int> &packetBestGuessLocation);
};

struct PromptInfo{
    std::string prompt;
    unsigned int seed;
};

struct StreamInfo{
    std::string stream;
};

class LetterSelector {
    public:
        virtual char selectCharacter() = 0;
};

/**
 * A basic randomized letter selector that uses the c++'s inbuilt mt19937 random number generator
 */
class mt19937LetterSelector : public LetterSelector {
    std::string alphabet;
    mt19937 rng;

    public:
        int seed;

        mt19937LetterSelector(std::string alphabet, int seed);
        char selectCharacter() override;
};

/**
 * A helper class for MonkeyTyper that handles counting how far up the query the stream is at.
 */
class PositionHolder{
    public:
        queue<int> currentSpot;
        int currentHighestSpot;
        int promptRecord;
        std::string query;
        bool complete;
        
        PositionHolder(std::string query);
        PositionHolder(std::string query, queue<int> &currentSpot, int promptRecord);
        void evaluateSelection(char selection);
        int getHighestSpot();
};

/**
 * A basic node for the system.
 * It generates a randomized stream of characters to 
 */
class MonkeyTyper {
    public:
        MonkeyTyper(int id, LetterSelector *rng, std::string query);
        MonkeyTyper(int id, LetterSelector *rng, std::string query, int packet_size);
        MonkeyTyper(int id, LetterSelector *rng, PositionHolder &currSpot, std::string query, int packet_size);
        /**
         * Randomly generates and evaluates charsMoved characters in a row.
         * It additionally fills up the packet variables to use when any Info() functions are called.
         * @param charsMoved: how many characters to generate and evaluate with the query.
         * @returns a status condition of the stream.
         */
        enum Status moveStream(int charsMoved);
        /**
         * Given a character, and assumed to know the stream of previous characters,
         * determine both all potential spots that character could be in, and the maximum possible spot.
         * @param selection the char to evaluate
         * @returns the maximum possible query position this selection could be.
         */
        int evaluateSelection(char selection);
        void pause();
        void unpause();
        void killStream();
        ListInfo listInfo();
        bool complete();
        int getPromptRecord();
        int getTotalStreamSize();
        std::string getGuessString();

    private:
        std::string query;
        LetterSelector* rng;
        int seed;
        int id;
        int totalStreamSize;
        int packetSize;
        vector<char> packetStream;
        vector<LetterOutcome> packetCorrectness;
        vector<char> packetCorrespondingQuery;
        vector<int> packetBestGuessLocation;
        std::string guessStream;
        PositionHolder currentSpot;
        atomic_bool isPaused;
        atomic_bool currentlyRunning;
        mutex startStreamLock;
};

#endif