#ifndef MONKEY_TYPER
#define MONKEY_TYPER

#include <string>
#include <queue>
#include <random>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
using namespace std;

extern std::string default_alphabet;

struct ListInfo{
    int id;
    int current_location_in_prompt;
    int guess_stream_size;
    int guess_distance_in_prompt_record;
    vector<char> prompt_corresponding_to_last_stream;
    vector<char> last_stream;
    vector<bool> last_stream_correctness;

    public:
        ListInfo(int id, int current_location_in_prompt,int guess_stream_size, int guess_distance_in_prompt_record, 
            vector<char> &prompt_corresponding_to_last_stream, vector<char> &last_stream,vector<bool> &last_stream_correctness);
};
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
 * A basic node for the system.
 * It generates a randomized stream of characters to 
 */
class MonkeyTyper {
    public:
        MonkeyTyper(int id, LetterSelector *rng, std::string query);
        MonkeyTyper(int id, LetterSelector *rng, std::string query, int packet_size);
        /**
         * Randomly generates and evaluates charsMoved characters in a row.
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

    private:
        std::string query;
        LetterSelector* rng;
        int seed;
        int id;
        bool completed;
        int promptRecord;
        int totalStreamSize;
        int packetSize;
        vector<char> packetStream;
        vector<LetterOutcome> packetCorrectness;
        vector<char> packetCorrespondingQuery;
        vector<int> packetBestGuessLocation;
        std::string guessStream;
        queue<int> currentSpot;
        atomic_bool isPaused;
        atomic_bool currentlyRunning;
        mutex startStreamLock;
};

#endif