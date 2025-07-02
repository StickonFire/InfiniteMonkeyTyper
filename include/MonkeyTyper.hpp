#include <string>
#include <queue>
#include <random>
using namespace std;

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

class MonkeyTyper {
    public:
        MonkeyTyper(int id, unsigned int seed, string query);
        MonkeyTyper(int id, mt19937 rng, string query);
        Status moveStream(int charsMoved);
        void startStream(queue<TyperMessage> &channel);
        void pause();
        void unpause();
        void killStream();

    private:
        string query;
        mt19937 rng;
        int seed;
        int id;
        int currentSpot;
};