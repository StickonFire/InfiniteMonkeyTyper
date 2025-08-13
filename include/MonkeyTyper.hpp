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

extern string default_alphabet;

struct ListInfo{
    int current_location_in_prompt;
    int guess_stream_size;
    int guess_distance_in_prompt_record;
    vector<char> prompt_corresponding_to_last_stream;
    vector<char> last_stream;
    vector<bool> last_stream_correctness;

    public:
        ListInfo(int current_location_in_prompt,int guess_stream_size, int guess_distance_in_prompt_record, 
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

class LetterSelector {
    public:
        virtual char selectCharacter() = 0;
};

class mt19937LetterSelector : public LetterSelector {
    string alphabet;
    mt19937 rng;

    public:
        int seed;

        mt19937LetterSelector(string alphabet, int seed);
        char selectCharacter() override;
};

class MonkeyTyper {
    public:
        MonkeyTyper(int id, LetterSelector *rng, string query);
        MonkeyTyper(int id, LetterSelector *rng, string query, int packet_size);
        enum Status moveStream(int charsMoved);
        void pause();
        void unpause();
        void killStream();
        ListInfo listInfo();

    private:
        string query;
        LetterSelector* rng;
        int seed;
        int id;
        bool completed;
        int charRecord;
        int currentLocation;
        int packet_size;
        vector<char> last_packet_stream;
        vector<bool> last_packet_correctness;
        vector<char> last_packet_corresponding_query_letters;
        string guess_stream;
        queue<int> currentSpot;
        atomic_bool isPaused;
        atomic_bool currentlyRunning;
        mutex startStreamLock;
};

#endif