#ifndef MOCKCLASS
#define MOCKCLASS

#include <random>
#include <gmock/gmock.h>
#include <iterator>

#include "MonkeyTyper.hpp"
#include "MonkeyTyperFactory.hpp"
#include "RingLeader.hpp"
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

#endif