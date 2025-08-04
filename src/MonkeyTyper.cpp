#include "MonkeyTyper.hpp"
#include <iterator>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

string default_alphabet = "abcdefghijklmnopqrstuvwxyz";

bool Status::operator==(const Status &other) const {
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

bool TyperMessage::operator==(const TyperMessage &other) const {
    return this->id == other.id && this->status == other.status;
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

bool TypedChar::operator==(const TypedChar &other) const {
    return this->letter == other.letter && this->position == other.position;
}

mt19937LetterSelector::mt19937LetterSelector(string alphabet,int seed): alphabet(alphabet), rng(seed), seed(seed) {}

char mt19937LetterSelector::selectCharacter(){
    int draw;
    do
    {
        draw = rng();
    } while (draw > alphabet.size() *(rng.max()/alphabet.size())); //Basically after the section where there isn't a whole number left.
    return alphabet[draw % alphabet.size()];
}

MonkeyTyper::MonkeyTyper(int id, LetterSelector* rng, string query) : query(query), rng(rng), seed(0), id(id), packet_size(8),completed(false), currentSpot() {
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

MonkeyTyper::MonkeyTyper(int id, LetterSelector* rng, string query, int packet_size) : query(query), rng(rng), seed(0), id(id), packet_size(packet_size),completed(false), currentSpot() {
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

Status MonkeyTyper::moveStream(int charsMoved){
    vector<TypedChar> typedChars;
    if(completed){
        return {typedChars,true};
    }
    char selection;
    int max;
    for(int i = 0; i < charsMoved; i++){
        selection = this->rng->selectCharacter();
        max = 0;
        this->currentSpot.push(0);
        int hold;

        for(int i = this->currentSpot.size(); i > 0; i--){
            hold = this->currentSpot.front();
            this->currentSpot.pop();
            if(selection == this->query[hold]){
                hold++;
                this->currentSpot.push(hold);
                if(hold > max)
                    max = hold;
                if(hold == this->query.size()){
                    completed = true;
                    typedChars.push_back(TypedChar{selection,max});
                    return Status{typedChars,true};
                }
            }
        }
        typedChars.push_back(TypedChar{selection,max});
    }
    return Status{typedChars,false};
}

thread MonkeyTyper::startStream(queue<TyperMessage> &channel){
    return thread(&MonkeyTyper::stream,this,ref(channel));
}

void MonkeyTyper::stream(queue<TyperMessage> &channel){
    startStreamLock.lock();
    if(currentlyRunning.load()){
        startStreamLock.unlock();
        return;
    }
    currentlyRunning.store(true);
    startStreamLock.unlock();

    Status result;
    while(!completed && currentlyRunning.load()){
        if(isPaused.load()){
            continue;
        }
        result = moveStream(packet_size);
        channel.push(TyperMessage{id,result});
    }
}


void MonkeyTyper::pause(){
    isPaused.store(true);
}
void MonkeyTyper::unpause(){
    isPaused.store(false);
}
void MonkeyTyper::killStream(){
    currentlyRunning.store(false);
}