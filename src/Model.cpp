#include "ModelInfo.hpp"
#include "RingLeader.hpp"
#include "Model.hpp"

Model::Model(std::unique_ptr<RingLeader> ringLeader, int runSize): ringLeader(std::move(ringLeader)), runSize(runSize) { }

ModelInfo Model::modelInfo(){
    std::map<int,std::string> placeHolderMessages;
    std::map<int,TyperInfo> typerInfos = this->ringLeader->typerInfo();
    ModelInfo result(placeHolderMessages,typerInfos);
    return result;
}

int Model::getRunSize(){
    return this->runSize;
}

void Model::setRunSize(int runSize){
    this->runSize = runSize;
}

void Model::doCreateMonkeyTyper(std::string prompt, unsigned int seed){
    this->ringLeader->createMonkeyTyper(prompt,seed);
}

void Model::doRemoveMonkeyTyper(unsigned int id){
    this->ringLeader->removeMonkeyTyper(id);
}

void Model::doPauseMonkeyTyper(unsigned int id){
    this->ringLeader->pauseMonkeyTyper(id);
}

void Model::doUnpauseMonkeyTyper(unsigned int id){
    this->ringLeader->unpauseMonkeyTyper(id);
}

void Model::doRun(){
    this->ringLeader->runNCharacters(runSize);
}