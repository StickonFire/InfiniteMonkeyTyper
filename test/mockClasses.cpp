#include "mockClasses.hpp"

MockLetterSelector::MockLetterSelector(vector<char> selections): LetterSelector(){
    returnValues = selections;
    itr = selections.begin();
}

char MockLetterSelector::selectCharacter(){
    char result = *itr;
    itr++;
    return result;
}