#include "MonkeyTyper.hpp"


MonkeyTyper::MonkeyTyper(int id, unsigned int seed, string query){
    this->query = query;
    this->id = id;
    this->rng = mt19937(seed);
    this->currentSpot = -1;
}

MonkeyTyper::MonkeyTyper(int id, mt19937 rng, string query){
    this->query = query;
    this->id = id;
    this->rng = rng;
    this->currentSpot = -1;
}

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