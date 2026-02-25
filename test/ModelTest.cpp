#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"
#include "Model.hpp"

#include <memory>
#include <numeric>
#include <vector>
#include <string>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::Test;
using ::testing::Mock;
using ::testing::_;

struct MonkeyTyperArguments{
    int id;
    std::unique_ptr<LetterSelector> letterSelector;
    std::string query;

    MonkeyTyperArguments(int id, std::unique_ptr<LetterSelector> letterSelector, std::string query): id(id), letterSelector(std::move(letterSelector)), query(query) { }
    MonkeyTyperArguments(MonkeyTyperArguments &&source): id(source.id), letterSelector(std::move(source.letterSelector)), query(source.query){ }
};

class ModelTest: public testing::Test {
    protected:
    unique_ptr<ModelInfo> expected;
    int runSize;
    unique_ptr<Model> test;
    
    void prepareTest(std::vector<MonkeyTyperArguments> &argumentList, unique_ptr<IdMaker> idGen, int runSize,unique_ptr<MonkeyTyperFactory> factory) {
        std::map<int,std::string> expectedMessages;
        std::map<int,TyperInfo> expectedTyper;
        expected = make_unique<ModelInfo>(expectedMessages,expectedTyper);
        std::map<int,MonkeyTyper> typers;
        for(int i = 0; i < argumentList.size(); i++){
            MonkeyTyper toAdd(argumentList[i].id,std::move(argumentList[i].letterSelector),argumentList[i].query);
            expected->typerValues.insert(std::make_pair(argumentList[i].id,toAdd.typerInfo()));
            typers.insert(std::make_pair(argumentList[i].id,std::move(toAdd)));
        }
        unique_ptr<RingLeader> modelRingLeader = std::make_unique<RingLeader>(typers,std::move(idGen),std::move(factory));
        this->runSize = runSize;
        test = make_unique<Model>(std::move(modelRingLeader),runSize);
    }

    void checkModelCorrectness(){
        EXPECT_EQ(test->modelInfo(),*expected);
        EXPECT_EQ(test->getRunSize(),runSize);
    }

    void addTyperTest(int id, unique_ptr<LetterSelector> letterSelector, std::string query){
        test->doCreateMonkeyTyper(query,letterSelector->getSeed());
        MonkeyTyper toAdd(id,std::move(letterSelector),query);
        expected->typerValues[id] = toAdd.typerInfo();
        checkModelCorrectness();
    }

    void removeTyperTest(int id){
        expected->typerValues.erase(id);
        test->doRemoveMonkeyTyper(id);
        checkModelCorrectness();
    }
};

TEST_F(ModelTest,ConstructorEmptyRingLeader){
    unique_ptr<IdMaker> idGenerator;
    int runSize = 1;
    std::vector<MonkeyTyperArguments> emptyList;
    prepareTest(emptyList,std::move(idGenerator),runSize,std::move(unique_ptr<MonkeyTyperFactory>()));
    checkModelCorrectness();
}

TEST_F(ModelTest,ConstructorSingleMonkeyTyper){
    unique_ptr<IdMaker> idGenerator;
    int runSize = 5;
    unique_ptr<MockLetterSelector> nullLetterSelector = make_unique<MockLetterSelector>();
    EXPECT_CALL(*nullLetterSelector,getSeed())
        .Times(1)
        .WillOnce(Return(20));
    std::vector<MonkeyTyperArguments> singleMonkey;
    singleMonkey.push_back(std::move(MonkeyTyperArguments(1,std::move(nullLetterSelector),std::string("ab"))));
    prepareTest(singleMonkey,std::move(idGenerator),runSize,std::move(unique_ptr<MonkeyTyperFactory>()));
    checkModelCorrectness();
}

TEST_F(ModelTest,RemoveOnlyTyper){
    int idToRemove = 9;
    unique_ptr<MockIdMaker> idGenerator = make_unique<MockIdMaker>();
    EXPECT_CALL(*idGenerator,releaseId(idToRemove))
        .Times(1);
    int runSize = 5;
    unique_ptr<MockLetterSelector> nullLetterSelector = make_unique<MockLetterSelector>();
    EXPECT_CALL(*nullLetterSelector,getSeed())
        .Times(1)
        .WillOnce(Return(20));
    std::vector<MonkeyTyperArguments> singleMonkey;
    singleMonkey.push_back(std::move(MonkeyTyperArguments(idToRemove,std::move(nullLetterSelector),std::string("ab"))));
    prepareTest(singleMonkey,std::move(idGenerator),runSize,std::move(unique_ptr<MonkeyTyperFactory>()));
    removeTyperTest(idToRemove);
}

TEST_F(ModelTest,RemoveNonExistentTyper){
    int idToRemove = 9;
    int runSize = 10;
    std::vector<MonkeyTyperArguments> noMonkeys;
    prepareTest(noMonkeys,std::move(make_unique<MockIdMaker>()),runSize,std::move(unique_ptr<MonkeyTyperFactory>()));
    removeTyperTest(idToRemove);
}

TEST_F(ModelTest,AddMonkeyToEmpty){
    int runSize = 10;
    int idToAdd = 45;
    unique_ptr<MockIdMaker> idGenerator = make_unique<MockIdMaker>();
    EXPECT_CALL(*idGenerator,generateId())
        .Times(1)
        .WillRepeatedly(Return(idToAdd));
    std::vector<MonkeyTyperArguments> emptyArgs;
    unique_ptr<MockMonkeyTyperFactory> factory = make_unique<MockMonkeyTyperFactory>();
    int seed = 10;
    unique_ptr<mt19937LetterSelector> letterSelector = make_unique<mt19937LetterSelector>("abc",seed);
    unique_ptr<mt19937LetterSelector> secondSelector = make_unique<mt19937LetterSelector>("abc",seed);
    MonkeyTyperArguments expectedNew(idToAdd,std::move(letterSelector),"abc");
    EXPECT_CALL(*factory,build(expectedNew.id,seed,expectedNew.query))
        .Times(1)
        .WillOnce(Return(MonkeyTyper(idToAdd,std::move(expectedNew.letterSelector),expectedNew.query)));
    prepareTest(emptyArgs,std::move(idGenerator),runSize,std::move(factory));
    addTyperTest(idToAdd,std::move(secondSelector),expectedNew.query);
}

TEST_F(ModelTest,AddMonkeyToOneTyperModel){
    int runSize = 10;
    int idToAdd = 45;
    unique_ptr<MockIdMaker> idGenerator = make_unique<MockIdMaker>();
    EXPECT_CALL(*idGenerator,generateId())
        .Times(1)
        .WillRepeatedly(Return(idToAdd));
    int existingId = 10;
    std::string query = "no";
    unique_ptr<mt19937LetterSelector> existingSelector = make_unique<mt19937LetterSelector>("no",55);
    std::vector<MonkeyTyperArguments> existingTypers;
    existingTypers.push_back(std::move(MonkeyTyperArguments(existingId,std::move(existingSelector),query)));
    unique_ptr<MockMonkeyTyperFactory> factory = make_unique<MockMonkeyTyperFactory>();
    int seed = 10;
    unique_ptr<mt19937LetterSelector> letterSelector = make_unique<mt19937LetterSelector>("abc",seed);
    unique_ptr<mt19937LetterSelector> expectedSelector = make_unique<mt19937LetterSelector>("abc",seed);
    MonkeyTyperArguments expectedNew(idToAdd,std::move(letterSelector),"abc");
    EXPECT_CALL(*factory,build(expectedNew.id,seed,expectedNew.query))
        .Times(1)
        .WillOnce(Return(MonkeyTyper(idToAdd,std::move(expectedNew.letterSelector),expectedNew.query)));
    prepareTest(existingTypers,std::move(idGenerator),runSize,std::move(factory));
    addTyperTest(idToAdd,std::move(expectedSelector),expectedNew.query);
}
