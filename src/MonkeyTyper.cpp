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


bool TypedChar::operator==(const TypedChar &other) const {
    return this->letter == other.letter && this->position == other.position;
}

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
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

MonkeyTyper::MonkeyTyper(int id, LetterSelector* rng, string query, int packet_size) : query(query), rng(rng), seed(0), id(id), packet_size(packet_size),completed(false), currentSpot() {
    this->isPaused.store(false);
    this->currentlyRunning.store(false);
}

enum Status MonkeyTyper::moveStream(int charsMoved){
    vector<TypedChar> typedChars;
    if(completed){
        return Completed;
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
                    return Completed;
                }
            }
        }
        typedChars.push_back(TypedChar{selection,max});
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