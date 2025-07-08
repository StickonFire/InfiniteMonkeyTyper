#include "MonkeyTyper.hpp"
#include <iterator>
#include <iostream>
#include <string>

string default_alphabet = "abcdefghijklmnopqrstuvwxyz";

bool Status::operator==(Status other) {
    return this->finished == other.finished && this->typeStream == other.typeStream;
}

ostream& operator<<(ostream& os, const Status &stat) {
    os << "Status:" << std::endl << "Finished:" << stat.finished << std::endl;
    os << "Sent Characters: " << std::endl;
    for(TypedChar tc: stat.typeStream){
        os << tc.letter << " ";
    }
    for(TypedChar tc: stat.typeStream){
        os << tc.position << " ";
    }
    return os;
}

string Status::toString(){
    string result = "Status: \nFinished:";
    result += (this->finished) ? "true":"false";
    result += "\nSent Characters: \n";
    for(TypedChar tc: this->typeStream){
        result += tc.letter + " ";
    }
    for(TypedChar tc: this->typeStream){
        result += to_string(tc.position) + " ";
    }
    return result;
}

bool TypedChar::operator==(const TypedChar other) const {
    return this->letter == other.letter && this->position == other.position;
}

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