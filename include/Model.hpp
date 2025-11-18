#ifndef MODEL
#define MODEL
#include "View.hpp"
#include "RingLeader.hpp"

#include <vector>

class Model {
    RingLeader ringLeader;

    void doRun();
    void doCreateMonkeyTyper(std::string,unsigned int seed);
    void doRemoveMonkeyTyper(unsigned int id);
    void doPauseMonkeyTyper(unsigned int id);
    void doUnpauseMonkeyTyper(unsigned int id);
};

#endif