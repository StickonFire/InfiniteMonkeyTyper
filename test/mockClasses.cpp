#include "mockClasses.hpp"

MockLetterSelector::MockLetterSelector(vector<char> selections): LetterSelector(){
    returnValues = selections;
    itr = returnValues.begin();
}

char MockLetterSelector::selectCharacter(){
    char result = *itr;
    itr++;
    return result;
}