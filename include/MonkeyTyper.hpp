#ifndef MONKEY_TYPER
#define MONKEY_TYPER

#include <string>
#include <queue>
#include <random>
#include <iostream>
using namespace std;

extern string default_alphabet;

struct TypedChar{
    char letter;
    int position;

    public:
        bool operator==(const TypedChar other) const;
};

struct Status {
    vector<TypedChar> typeStream;
    bool finished;

    public:
        bool operator==(Status other);
        friend ostream& operator<<(ostream& os, const TypedChar& stat);
        string toString();
};

struct TyperMessage {
    int id;
    Status status;
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
        Status moveStream(int charsMoved);
        void startStream(queue<TyperMessage> &channel);
        void pause();
        void unpause();
        void killStream();

    private:
        string query;
        LetterSelector* rng;
        int seed;
        int id;
        bool completed;
        queue<int> currentSpot;
};

#endif