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

ListInfo::ListInfo(int id, int current_loc,int guess_stream_size, int guess_distance_in_prompt_record, 
            vector<char> &prompt_corresponding_to_last_stream, vector<char> &last_stream, vector<bool> &correctness): 
                id(id), current_location_in_prompt(current_loc), guess_stream_size(guess_stream_size){
    
    this->prompt_corresponding_to_last_stream = prompt_corresponding_to_last_stream;
    this->last_stream = last_stream;
    this->last_stream_correctness = correctness;
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
PositionHolder::PositionHolder(std::string query): currentSpot(), currentHighestSpot(0), promptRecord(0), query(query) { }

PositionHolder::PositionHolder(std::string query, queue<int> &currentSpot, int promptRecord): currentSpot(currentSpot), currentHighestSpot(0), promptRecord(promptRecord), query(query) { }

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
        }
    }
}

    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

MonkeyTyper::MonkeyTyper(int id, LetterSelector* rng, string query, int packet_size) : query(query), rng(rng), seed(0), id(id), packet_size(packet_size),completed(false), currentSpot() {
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

int MonkeyTyper::evaluateSelection(char selection){
    guessStream += selection;
    totalStreamSize++;

    int currentMax = 0;
    this->currentSpot.push(0);

    for(int current_spot_itr = this->currentSpot.size(); current_spot_itr > 0; current_spot_itr--){
        int hold = this->currentSpot.front();
        this->currentSpot.pop();
        if(selection == this->query[hold]){
            hold++;
            this->currentSpot.push(hold);
            if(hold > currentMax){
                currentMax = hold;
                if(currentMax > promptRecord){
                    promptRecord = currentMax;
                }
            }
            if(hold == this->query.size()){
                this->completed = true;
                return this->query.size();
            }
        }
    }
    return currentMax;
}

enum Status MonkeyTyper::moveStream(int charsMoved){
    vector<TypedChar> typedChars;
    if(completed){
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
        if(completed){
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

ListInfo MonkeyTyper::listInfo(){
    return ListInfo(id, currentLocation,guess_stream.size(),charRecord, 
            last_packet_corresponding_query_letters,last_packet_stream,last_packet_correctness);
}