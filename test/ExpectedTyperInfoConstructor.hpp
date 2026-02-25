#ifndef SLICER
#define SLICER
#include "ModelInfo.hpp"

#include <vector>

template<typename Contained>
struct VectorSlicer{
    std::vector<Contained> container;
    typename std::vector<Contained>::iterator containerItr;

    VectorSlicer(std::vector<Contained> container): container(container), containerItr(this->container.begin()) { }

    std::vector<Contained> slice(int size){
        typename std::vector<Contained>::iterator end = containerItr;
        end += size;
        std::vector<Contained> result(containerItr,end);
        containerItr = end;
        return result;
    }
};

class ExpectedListInfoConstructor{
    int id;
    VectorSlicer<char> expectedStream;
    VectorSlicer<char> expectedCorrespondingQuery;
    VectorSlicer<LetterOutcome> expectedOutcome;
    VectorSlicer<int> expectedBestLocation;

    VectorSlicer<int> expectedCurrentLocation;
    VectorSlicer<int> expectedRecord;
    VectorSlicer<int> expectedSize;

    public:
        ExpectedListInfoConstructor(int id,std::vector<int> expectedCurrentLocation, std::vector<int> expectedSize, std::vector<int> expectedRecord, std::vector<char> expectedStream, std::vector<LetterOutcome> expectedOutcome, std::vector<char> expectedCorrespondingQuery, 
            std::vector<int> expectedBestLocation);

        ListInfo generateNextListInfo(int size);

        ListInfo generateEmptyListInfo();
};

class ExpectedTyperInfoConstructor{
    VectorSlicer<std::string> streams;
    std::string prompt;
    unsigned int seed;
    
    public:
        ExpectedTyperInfoConstructor(std::vector<std::string> streams,std::string prompt,unsigned int seed);

        TyperInfo generateNextTyperInfo(ListInfo listInfo);

        TyperInfo generateEmptyTyperInfo(int id);
};

#endif