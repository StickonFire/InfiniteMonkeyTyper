#include <string>
#include <queue>
#include <random>
using namespace std;

string default_alphabet = "abcdefghijklmnopqrstuvwxyz";

struct TypedChar{
    char letter;
    int position;
};

struct Status {
    vector<TypedChar> typeStream;
    bool finished;
};

struct TyperMessage {
    int id;
    Status status;
};

class LetterSelector {
    string alphabet;
    mt19937 rng;

    public:
        int seed;

        LetterSelector(string alphabet, int seed);
        char selectCharacter();
};

class MonkeyTyper {
    public:
        MonkeyTyper(int id, unsigned int seed, string query);
        MonkeyTyper(int id, LetterSelector rng, string query);
        Status moveStream(int charsMoved);
        void startStream(queue<TyperMessage> &channel);
        void pause();
        void unpause();
        void killStream();

    private:
        string query;
        LetterSelector rng;
        int seed;
        int id;
        vector<int> currentSpot;
};