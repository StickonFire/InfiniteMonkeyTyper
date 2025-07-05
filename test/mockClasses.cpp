#include "mockClasses.hpp"

MockLetterSelector::MockLetterSelector(vector<char> selections): LetterSelector("",0){
    returnValues = selections;
    itr = selections.begin();
}

char MockLetterSelector::selectCharacter(){
    char result = *itr;
    itr++;
    return result;
}