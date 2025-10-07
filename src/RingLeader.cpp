#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"

#include <set>

CounterIdMaker::CounterIdMaker(int startCounter, std::set<int> usedIds): counter(startCounter), usedIds(usedIds) { }


int CounterIdMaker::getCounter(){
    return this->counter;
}

std::set<int> CounterIdMaker::getUsedIds(){
    return this->usedIds;
}

vector<MonkeyTyperStatus> RingLeader::runNCharacters(int n){
    return vector<MonkeyTyperStatus>();
}

vector<IdAndListInfo> RingLeader::listInfo(){
    return vector<IdAndListInfo>();
}

StreamInfo RingLeader::streamInfo(int id){
    return StreamInfo();
}

PromptInfo RingLeader::promptInfo(int id){
    return PromptInfo();
}

int RingLeader::createMonkeyTyper(std::string query, int seed){
    return 0;
}

void RingLeader::pauseMonkeyTyper(int id){

}

void RingLeader::unpauseMonkeyTyper(int id){

}

void RingLeader::removeMonkeyTyper(int id){

}