#include "MonkeyTyper.hpp"

LetterSelector::LetterSelector(string alphabet,int seed): alphabet(alphabet), rng(seed), seed(seed) {}

char LetterSelector::selectCharacter(){
    int draw;
    do
    {
        draw = rng();
    } while (draw > alphabet.size() *(rng.max()/alphabet.size())); //Basically after the section where there isn't a whole number left.
    return alphabet[draw % alphabet.size()];
}

MonkeyTyper::MonkeyTyper(int id, unsigned int seed, string query) : query(query),rng(default_alphabet,seed), seed(seed), id(id), currentSpot() { }

MonkeyTyper::MonkeyTyper(int id, LetterSelector rng, string query) : query(query), rng(rng), seed(rng.seed), id(id), currentSpot()  {  }

Status MonkeyTyper::moveStream(int charsMoved){
    return Status{

    };
}

void MonkeyTyper::startStream(queue<TyperMessage> &channel){
    
}
void MonkeyTyper::pause(){

}
void MonkeyTyper::unpause(){

}
void MonkeyTyper::killStream(){

}