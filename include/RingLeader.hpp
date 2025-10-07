#ifndef RingLeaderHeader
#define RingLeaderHeader
#include "MonkeyTyper.hpp"

#include <vector>
#include <map>
#include <memory>
#include <set>

struct MonkeyTyperStatus {
    int id;
    Status status;
};

struct IdAndListInfo {
    int id;
    ListInfo listInfo;
};

class IdMaker{

    public:
        virtual ~IdMaker() = default;
        virtual int generateId() = 0;
        virtual void releaseId(int toRelease) = 0;
};

/**
 * A simple IdMaker that merely increments the id given every time an id is asked for.
 */
class CounterIdMaker : IdMaker{
    unsigned int counter;
    std::set<int> usedIds;

    public:
        /**
         * Generrates an idMaker that assigns an id numerically in order.
         * @param startCounter first id to use and where the counter to assign ids start.
         */
        CounterIdMaker(int startCounter, std::set<int> usedIds);
        /**
         * Generates a unique id. 
         */
        int generateId();
        /**
         * This function releases the id provided, allowing it to be used again.
         */
        void releaseId(int toRelease);
};

/**
 * A class to handle an aggregation of MonkeyTypers.
 */
class RingLeader {
    std::map<int,MonkeyTyper> typers;
    unique_ptr<IdMaker> idGenerator;

    public:
        /**
         * Given an integer n, runs each of its typers n characters forward, unless all typers reach completion.
         * @param n the number of characters each typer runs, unless they are paused or completed.
         * @return a vector of pairs, containing an id and status of each monkeyTyper.
         */
        vector<MonkeyTyperStatus> runNCharacters(int n);
        /**
         * Extracts from the typers a vector of ids and ListInfo of each typer.
         */
        vector<IdAndListInfo> listInfo();
        /**
         * Given a valid id, returns a stream info of the monkeyTyper associated with that id.
         */
        StreamInfo streamInfo(int id);
        PromptInfo promptInfo(int id);
        /**
         * Generates a monkeyTyper using the query and the seed. It additionally assigns an id to the MonkeyTyper.
         * @return id of the generated MonkeyTyper
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