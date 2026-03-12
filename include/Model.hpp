#ifndef MODEL
#define MODEL
#include "ModelInfo.hpp"
#include "RingLeader.hpp"

#include <string>
#include <memory>

class AbstractModel {
    public:
        virtual void doRun() = 0;
        virtual void doCreateMonkeyTyper(std::string query,unsigned int seed) = 0;
        virtual void doRemoveMonkeyTyper(unsigned int id) = 0;
        virtual void doPauseMonkeyTyper(unsigned int id) = 0;
        virtual void doUnpauseMonkeyTyper(unsigned int id) = 0;
        virtual ModelInfo modelInfo() = 0;
        virtual int getRunSize() = 0;
        virtual void setRunSize(int runSize) = 0;
};

class Model : public AbstractModel {
    std::unique_ptr<RingLeader> ringLeader;
    int runSize;

    public:
        Model(std::unique_ptr<RingLeader> ringLeader, int runSize);
        void doRun() override;
        void doCreateMonkeyTyper(std::string query,unsigned int seed) override;
        void doRemoveMonkeyTyper(unsigned int id) override;
        void doPauseMonkeyTyper(unsigned int id) override;
        void doUnpauseMonkeyTyper(unsigned int id) override;
        ModelInfo modelInfo() override;
        int getRunSize() override;
        void setRunSize(int runSize) override;
};

#endif