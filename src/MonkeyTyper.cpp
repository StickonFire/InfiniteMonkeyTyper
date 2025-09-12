#include "MonkeyTyper.hpp"
#include <iterator>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>



bool TypedChar::operator==(const TypedChar &other) const {
    return this->letter == other.letter && this->position == other.position;
}
std::string default_alphabet = "abcdefghijklmnopqrstuvwxyz";

mt19937LetterSelector::mt19937LetterSelector(string alphabet,int seed): alphabet(alphabet), rng(seed), seed(seed) {}

char mt19937LetterSelector::selectCharacter(){
    int draw;
    do
    {
        draw = rng();
    } while (draw > alphabet.size() *(rng.max()/alphabet.size())); //Basically after the section where there isn't a whole number left.
    return alphabet[draw % alphabet.size()];
}

PositionHolder::PositionHolder(std::string query): currentSpot(), currentHighestSpot(0), promptRecord(0), query(query), complete(false) { }

PositionHolder::PositionHolder(std::string query, queue<int> &currentSpot, int promptRecord): currentSpot(currentSpot), currentHighestSpot(0), promptRecord(promptRecord), query(query), complete(false) { }

int PositionHolder::getHighestSpot(){
    return this->currentHighestSpot;
}

void PositionHolder::evaluateSelection(char selection){
    this->currentSpot.push(0);
    currentHighestSpot = 0;
    for(int current_spot_itr = this->currentSpot.size(); current_spot_itr > 0; current_spot_itr--){
        int hold = this->currentSpot.front();
        this->currentSpot.pop();
        if(selection == this->query[hold]){
            hold++;
            this->currentSpot.push(hold);
            if(hold > currentHighestSpot){
                currentHighestSpot = hold;
                if(currentHighestSpot > promptRecord){
                    promptRecord = currentHighestSpot;
                }
            }
            if(currentHighestSpot == query.size()){
                this->complete = true;
                return;
            }
        }
    }
}

MonkeyTyper::MonkeyTyper(int id, LetterSelector* rng, string query) : query(query), rng(rng), seed(0), id(id), packetSize(8),currentSpot(query),
        totalStreamSize(0),packetStream(),packetBestGuessLocation(),packetCorrespondingQuery(),packetCorrectness(){
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

MonkeyTyper::MonkeyTyper(int id, LetterSelector* rng, string query, int packet_size) : query(query), rng(rng), seed(0), id(id), packetSize(packet_size), currentSpot(query),
        totalStreamSize(0),packetStream(),packetBestGuessLocation(),packetCorrespondingQuery(),packetCorrectness() {
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

MonkeyTyper::MonkeyTyper(int id, LetterSelector* rng, PositionHolder &currentSpot, string query, int packet_size) : query(query), rng(rng), seed(0), id(id), packetSize(packet_size), currentSpot(currentSpot),
        totalStreamSize(0),packetStream(),packetBestGuessLocation(),packetCorrespondingQuery(),packetCorrectness() {
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

int MonkeyTyper::evaluateSelection(char selection){
    guessStream += selection;
    totalStreamSize++;

    currentSpot.evaluateSelection(selection);
    return currentSpot.getHighestSpot();
}

enum Status MonkeyTyper::moveStream(int charsMoved){
    vector<TypedChar> typedChars;
    if(complete()){
        return Completed;
    }
    else if (isPaused){
        return Paused;
    }

    int prevMax;
    if(packetBestGuessLocation.size() == 0){
        prevMax = 0;
    } else {
        prevMax = packetBestGuessLocation.back();
    }
    
    packetBestGuessLocation = vector<int>(packetSize,0);
    packetStream = vector<char>(packetSize,'a');
    packetCorrespondingQuery = vector<char>(packetSize,'a');
    packetCorrectness = vector<LetterOutcome>(packetSize,NoMatch);

    //Loops through the entire packet.
    for(int packet_spot = 0; packet_spot < charsMoved; packet_spot++){
        char selection = this->rng->selectCharacter();
        packetStream[packet_spot] = selection;
        
        int currentMax = evaluateSelection(selection);
        
        if(currentMax == prevMax+1){
            packetCorrectness[packet_spot] = Match;
        }
        else if(currentMax != 0){
            packetCorrectness[packet_spot] = Fallback;
        }
        else {
            packetCorrectness[packet_spot] = NoMatch;
        }
        packetBestGuessLocation[packet_spot] = currentMax;
        prevMax = currentMax;
        if(complete()){
            return Completed;
        }
    }
    return PacketReady;
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

bool MonkeyTyper::complete(){
    return currentSpot.complete;
}

std::string MonkeyTyper::getGuessString(){
    return guessStream;
}

int MonkeyTyper::getTotalStreamSize(){
    return totalStreamSize;
}

int MonkeyTyper::getPromptRecord(){
    return currentSpot.promptRecord;
}