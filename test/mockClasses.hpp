#ifndef MOCKCLASS
#define MOCKCLASS

#include <random>
#include <gmock/gmock.h>
#include <iterator>

#include "MonkeyTyper.hpp"
using namespace std;

class MockLetterSelector : public LetterSelector {
    public:
        MOCK_METHOD(char, selectCharacter, (), (override));
        MOCK_METHOD(unsigned int, getSeed, (), (override));
};

#endif