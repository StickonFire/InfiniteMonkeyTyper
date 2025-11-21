#include "ModelInfo.hpp"

#include <vector>
#include <iostream>
#include <ostream>

ListInfo::ListInfo(): id(0), currentLocation(0), guessStreamSize(0), promptRecord(0), packetStream(), 
    packetCorrectness(), packetCorrespondingQuery(), packetBestGuessLocation() { }
  
ListInfo::ListInfo(int id, int currentLocation,int guessStreamSize, int promptRecord, std::vector<char> &packetStream, 
    std::vector<LetterOutcome> &packetCorrectness, std::vector<char> &packetCorrespondingQuery, std::vector<int> &packetBestGuessLocation){
    
    this->id = id;
    this->currentLocation = currentLocation;
    this->guessStreamSize = guessStreamSize;
    this->promptRecord = promptRecord;
    this->packetStream = packetStream;
    this->packetCorrectness = packetCorrectness;
    this->packetCorrespondingQuery = packetCorrespondingQuery;
    this->packetBestGuessLocation = packetBestGuessLocation;
}

std::ostream& operator<<(std::ostream &os, const ListInfo add){
    os << "List Info:" << std::endl;
    os << "id: " << add.id << std::endl;
    os << "currentLocation: " << add.currentLocation << std::endl;
    os << "guessStreamSize: " << add.guessStreamSize << std::endl;
    os << "promptRecord: " << add.promptRecord << std::endl;
    std::string packetStream             = "packetStream:             {";
    std::string packetCorrectness        = "packetCorrectness:        {";
    std::string packetCorrespondingQuery = "packetCorrespondingQuery: {";
    std::string packetBestGuessLocation  = "packetBestGuessLocation:  {";
    for(int i = 0; i < add.packetStream.size(); i++){
        packetStream += add.packetStream[i];
        packetCorrectness += std::to_string(add.packetCorrectness[i]);
        packetCorrespondingQuery += add.packetCorrespondingQuery[i];
        packetBestGuessLocation += std::to_string(add.packetBestGuessLocation[i]);
    }
    os << packetStream << "}" << std::endl;
    os << packetCorrectness << "}" << std::endl;
    os << packetCorrespondingQuery << "}" << std::endl;
    os << packetBestGuessLocation << "}" << std::endl;
    os << "}";
    return os;
}

bool ListInfo::operator==(const ListInfo &other) const {
    return this->id == other.id
        && this->currentLocation == other.currentLocation
        && this->guessStreamSize == other.guessStreamSize
        && this->promptRecord == other.promptRecord
        && this->packetStream == other.packetStream
        && this->packetCorrectness == other.packetCorrectness
        && this->packetCorrespondingQuery == other.packetCorrespondingQuery
        && this->packetBestGuessLocation == other.packetBestGuessLocation;
}