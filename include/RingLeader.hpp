#ifndef RingLeaderHeader
#define RingLeaderHeader
#include "MonkeyTyper.hpp"

#include <vector>

/**
 * A class to handle an aggregation of MonkeyTypers.
 */
class RingLeader {
    vector<MonkeyTyper> typers;

    public:
        /**
         * Given an integer n, runs each of its typers n characters forward, unless all typers reach completion.
         * @param n the number of characters each typer runs, unless they are paused or completed.
         * @return a vector of pairs, containing an id and status of each monkeyTyper.
         */
        vector<pair<int,Status>> runNCharacters(int n);
        /**
         * Extracts from the typers a vector of ids and ListInfo of each typer.
         */
        vector<pair<int,ListInfo>> listInfo(void);
        /**
         * Given a valid id, returns a stream info of the monkeyTyper associated with that id.
         */
        StreamInfo streamInfo(int id);
        PromptInfo promptInfo(int id);
        /**
         * Generates a monkeyTyper using the query and the seed. It additionally assigns an id to the MonkeyTyper.
         */
        int createMonkeyTyper(std::string query, int seed);
        void pauseMonkeyTyper(int id);
        void unpauseMonkeyTyper(int id);
        /**
         * Removes the MonkeyTyper with the given id.
         */
        void removeMonkeyTyper(int id);
};

#endif