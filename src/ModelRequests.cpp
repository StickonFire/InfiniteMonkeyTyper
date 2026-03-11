#include "Model.hpp"
#include "ModelRequests.hpp"

PauseRequest::PauseRequest(unsigned int id) {
    this->id = id;
}

void PauseRequest::dispatch(Model *target) {
    target->doPauseMonkeyTyper(id);
}

UnpauseRequest::UnpauseRequest(unsigned int id) {
    this->id = id;
}

void UnpauseRequest::dispatch(Model *target) {
    target->doUnpauseMonkeyTyper(id);
}

RemoveRequest::RemoveRequest(unsigned int id) {
    this->id = id;
}

void RemoveRequest::dispatch(Model *target) {
    target->doRemoveMonkeyTyper(id);
}

CreateRequest::CreateRequest(std::string query, unsigned int seed){
    this->seed = seed;
    this->query = query;
}

void CreateRequest::dispatch(Model *target){
    target->doCreateMonkeyTyper(query,seed);
}

