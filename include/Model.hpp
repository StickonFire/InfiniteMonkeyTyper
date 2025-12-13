#ifndef MODEL
#define MODEL
#include "ModelInfo.hpp"
#include "RingLeader.hpp"

#include <string>
#include <memory>

class Model {
    std::unique_ptr<RingLeader> ringLeader;

    public:
        Model(std::unique_ptr<RingLeader> ringLeader);
        void doRun();
        void doCreateMonkeyTyper(std::string,unsigned int seed);
        void doRemoveMonkeyTyper(unsigned int id);
        void doPauseMonkeyTyper(unsigned int id);
        void doUnpauseMonkeyTyper(unsigned int id);
        ModelInfo modelInfo();
};

#endif