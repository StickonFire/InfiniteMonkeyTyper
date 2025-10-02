#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"

vector<MonkeyTyperStatus> RingLeader::runNCharacters(int n){
    return vector<MonkeyTyperStatus>();
}

vector<IdAndListInfo> RingLeader::listInfo(){
    return vector<IdAndListInfo>();
}

StreamInfo RingLeader::streamInfo(int id){
    if(typers.find(id) != typers.end())
        return typers[id].streamInfo();
}

PromptInfo RingLeader::promptInfo(int id){
    if(typers.find(id) != typers.end())
        return typers[id].promptInfo();
}

int RingLeader::createMonkeyTyper(std::string query, int seed){
    unique_ptr<LetterSelector> selector = make_unique<mt19937LetterSelector>(seed);
    int id = idGenerator->generateId();
    this->typers[id] = MonkeyTyper(id,std::move(selector),query);
    return id;
}

void RingLeader::pauseMonkeyTyper(int id){

}

void RingLeader::unpauseMonkeyTyper(int id){

}

void RingLeader::removeMonkeyTyper(int id){

}