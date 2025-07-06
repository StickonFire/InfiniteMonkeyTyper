#ifndef MOCKCLASS
#define MOCKCLASS

#include <random>
#include "MonkeyTyper.hpp"
#include <iterator>
using namespace std;

class MockLetterSelector : public LetterSelector {
    public:
        MockLetterSelector(vector<char> returnValues);
        char selectCharacter();

    private:
        vector<char> returnValues;
        vector<char>::iterator itr;
};

#endif