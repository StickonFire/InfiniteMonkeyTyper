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

