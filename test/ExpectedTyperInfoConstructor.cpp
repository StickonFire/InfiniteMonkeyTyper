#include "ExpectedTyperInfoConstructor.hpp"
#include "ModelInfo.hpp"

#include <vector>

ExpectedListInfoConstructor::ExpectedListInfoConstructor(int id,std::vector<int> expectedCurrentLocation, std::vector<int> expectedSize, std::vector<int> expectedRecord, std::vector<char> expectedStream, std::vector<LetterOutcome> expectedOutcome, std::vector<char> expectedCorrespondingQuery, 
    std::vector<int> expectedBestLocation) :
    id(id), expectedStream(expectedStream), expectedCorrespondingQuery(expectedCorrespondingQuery), expectedOutcome(expectedOutcome), expectedBestLocation(expectedBestLocation),
    expectedCurrentLocation(expectedCurrentLocation), expectedRecord(expectedRecord), expectedSize(expectedSize) { };

ListInfo ExpectedListInfoConstructor::generateNextListInfo(int size){
    std::vector<char> stream = expectedStream.slice(size);
    std::vector<LetterOutcome> outcome = expectedOutcome.slice(size);
    std::vector<char> corresponding = expectedCorrespondingQuery.slice(size);
    std::vector<int> bestLocation = expectedBestLocation.slice(size);
    int currentLocation = expectedCurrentLocation.slice(1)[0];
    int streamSize = expectedSize.slice(1)[0];
    int record = expectedRecord.slice(1)[0];
    return ListInfo(id,currentLocation,streamSize,record,stream,outcome,corresponding,bestLocation);
}

ListInfo ExpectedListInfoConstructor::generateEmptyListInfo(){
    std::vector<char> emptyChar;
    std::vector<LetterOutcome> emptyOutcome;
    std::vector<int> emptyInt;
    return ListInfo(id,0,0,0,emptyChar,emptyOutcome,emptyChar,emptyInt);
}

ExpectedTyperInfoConstructor::ExpectedTyperInfoConstructor(std::vector<std::string> streams,std::string prompt,unsigned int seed):
    streams(streams),prompt(prompt),seed(seed) { }

TyperInfo ExpectedTyperInfoConstructor::generateNextTyperInfo(ListInfo listInfo){
    return TyperInfo(listInfo,streams.slice(1)[0],prompt,seed);
}

TyperInfo ExpectedTyperInfoConstructor::generateEmptyTyperInfo(int id){
    ListInfo empty;
    empty.id = id;
    return TyperInfo(empty,std::string(""),prompt,seed);
}
