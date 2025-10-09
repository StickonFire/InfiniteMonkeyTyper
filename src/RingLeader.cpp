#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"

#include <set>
#include <optional>
#include <map>

CounterIdMaker::CounterIdMaker(int startCounter, std::set<int> usedIds): counter(startCounter), usedIds(usedIds) { }

int CounterIdMaker::generateId(){
    while(usedIds.find(counter) != usedIds.end()){
        counter++;
    }
    int id = counter;
    usedIds.insert(id);
    counter++;
    return id;
}

void CounterIdMaker::releaseId(int id){
    if(usedIds.find(id) != usedIds.end())
        usedIds.erase(usedIds.find(id));
}

int CounterIdMaker::getCounter(){
    return this->counter;
}

std::set<int> CounterIdMaker::getUsedIds(){
    return this->usedIds;
}

RingLeader::RingLeader(std::map<int,MonkeyTyper>& typers, unique_ptr<IdMaker> idmaker): typers(), idGenerator(std::move(idmaker)) {
    for(std::map<int,MonkeyTyper>::iterator itr = typers.begin(); itr != typers.end(); itr++){
        this->typers.insert(std::make_pair(itr->first,std::move(itr->second)));
    }
}

vector<MonkeyTyperStatus> RingLeader::runNCharacters(int n){
    return vector<MonkeyTyperStatus>();
}

vector<IdAndListInfo> RingLeader::listInfo(){
    return vector<IdAndListInfo>();
}

std::optional<StreamInfo> RingLeader::streamInfo(int id){
    return StreamInfo();
}

std::optional<PromptInfo> RingLeader::promptInfo(int id){
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