#ifndef MOCKCLASS
#define MOCKCLASS

#include "MonkeyTyper.hpp"
#include "MonkeyTyperFactory.hpp"
#include "RingLeader.hpp"
#include "Model.hpp"

#include <random>
#include <gmock/gmock.h>
#include <iterator>
using namespace std;

class MockLetterSelector : public LetterSelector {
    public:
        MOCK_METHOD(char, selectCharacter, (), (override));
        MOCK_METHOD(unsigned int, getSeed, (), (override));
};

class MockIdMaker : public IdMaker {
    public:
        MOCK_METHOD(int, generateId, (), (override));
        MOCK_METHOD(void, releaseId, (int id), (override));
};


class MockMonkeyTyperFactory : public MonkeyTyperFactory {
    public:
        MOCK_METHOD(MonkeyTyper,build, (int id, unsigned int seed, std::string query), (override));
};

class MockModel : public AbstractModel {
    public:
        MOCK_METHOD(void,doRun,(),(override));
        MOCK_METHOD(void,doCreateMonkeyTyper,(std::string query,unsigned int seed),(override));
        MOCK_METHOD(void,doRemoveMonkeyTyper,(unsigned int id),(override));
        MOCK_METHOD(void,doPauseMonkeyTyper,(unsigned int id),(override));
        MOCK_METHOD(void,doUnpauseMonkeyTyper,(unsigned int id),(override));
        MOCK_METHOD(ModelInfo,modelInfo,(),(override));
        MOCK_METHOD(int,getRunSize,(),(override));
        MOCK_METHOD(void,setRunSize,(int runSize),(override));
};

#endif