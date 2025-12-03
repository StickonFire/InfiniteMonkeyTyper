#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"

#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::Test;
using ::testing::Mock;
using ::testing::_;

template <class T>
ostream& operator<<(ostream& os, vector<T> other){
    os << "vector: {";
    for(T element : other){
        os << element << " ";
    }
    os << "}";
    return os;
}

namespace {
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
}

class RingLeaderWholisticTestSuite : public testing::Test {

    protected:
        std::vector<ListInfo> expectedListInfo;
        std::map<int,TyperInfo> expectedTyperInfo;
        std::vector<MonkeyTyperStatus> expectedStatus;

        void checkInfoStructs(RingLeader &test, std::string message){
            EXPECT_EQ(test.listInfo(),expectedListInfo) << message << "ListInfo failed to match!";
            EXPECT_EQ(test.typerInfo(),expectedTyperInfo) << message << "TyperInfos failed to match!";
        }
};

/**
 * Wholistic tests for the following functions:
 *  - Constructor
 *  - runNCharacters
 *  - listInfo
 *  - promptInfo
 *  - streamInfo
 */
TEST(SlicerTest,SliceTwice){
    vector<int> toSlice{0,1,2};
    VectorSlicer<int> test(toSlice);

    EXPECT_EQ(test.slice(1),vector<int>{0});
    EXPECT_EQ(test.slice(2),(vector<int>{1,2}));
}

TEST_F(RingLeaderWholisticTestSuite,EmptyList){
    std::map<int,MonkeyTyper> empty;
    std::set<int> usedIds;
    unique_ptr<CounterIdMaker> idGenerator = make_unique<CounterIdMaker>(50,usedIds);
    RingLeader test(empty,std::move(idGenerator));

    checkInfoStructs(test,"Case no monkeyTypers, and no running.");

    EXPECT_EQ(test.runNCharacters(100),expectedStatus);

    checkInfoStructs(test,"Case no monkeyTypers, and has run typers once.");
}

TEST_F(RingLeaderWholisticTestSuite,OneMonkeyTyper){
    std::map<int,MonkeyTyper> onlyOne;
    unique_ptr<MockLetterSelector> selector = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector,selectCharacter())
        .Times(2)
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));
    MonkeyTyper onlyTyper(1,std::move(selector),"ab");
    onlyTyper.moveStream(1);
    onlyOne.insert(std::make_pair(1,std::move(onlyTyper)));

    std::set<int> usedIds;
    unique_ptr<CounterIdMaker> idGenerator = make_unique<CounterIdMaker>(50,usedIds);
    vector<char> expectedStream{'a'};
    vector<LetterOutcome> expectedOutcome{Match};
    vector<char> expectedQuery{'a'};
    vector<int> expectedPosition{1};
    ListInfo expectedInfo(1,1,1,1,expectedStream,expectedOutcome,expectedQuery,expectedPosition);
    RingLeader test(onlyOne,std::move(idGenerator));
    
    vector<ListInfo> expectedListInfo(1,expectedInfo);
    unsigned int seed = 0;
    StreamInfo expectedStreamInfo(seed,"a",expectedInfo);
    PromptInfo expectedPromptInfo(seed,"ab",expectedInfo);

    EXPECT_EQ(test.listInfo(),expectedListInfo);
    EXPECT_EQ(*(test.streamInfo(1)),expectedStreamInfo);
    EXPECT_EQ(*(test.promptInfo(1)),expectedPromptInfo);

    vector<MonkeyTyperStatus> expectedStatus{MonkeyTyperStatus(1,Completed)};
    EXPECT_EQ(test.runNCharacters(1),expectedStatus);

    expectedStream = vector<char>{'b'};
    expectedOutcome = vector<LetterOutcome>{Complete};
    expectedQuery = vector<char>{'b'};
    expectedPosition = vector<int>{2};
    expectedInfo = ListInfo(1,2,2,2,expectedStream,expectedOutcome,expectedQuery,expectedPosition);
    expectedListInfo = vector<ListInfo>{expectedInfo};
    expectedStreamInfo = StreamInfo(seed,"ab",expectedInfo);
    expectedPromptInfo = PromptInfo(seed,"ab",expectedInfo);

    EXPECT_EQ(test.listInfo(),expectedListInfo);
    EXPECT_EQ(*(test.streamInfo(1)),expectedStreamInfo);
    EXPECT_EQ(*(test.promptInfo(1)),expectedPromptInfo);

    //Test for nonexistant ids
    EXPECT_FALSE(test.streamInfo(2));
    EXPECT_FALSE(test.promptInfo(2));
}

TEST_F(RingLeaderWholisticTestSuite,TwoMonkeysOneUnrun){
    std::map<int,MonkeyTyper> two;
    unique_ptr<MockLetterSelector> firstSelector = make_unique<MockLetterSelector>();
    unique_ptr<MockLetterSelector> secondSelector = make_unique<MockLetterSelector>();

    EXPECT_CALL(*firstSelector,selectCharacter())
        .Times(2)
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));
    EXPECT_CALL(*firstSelector,getSeed())
        .Times(1)
        .WillOnce(Return(2));
    EXPECT_CALL(*secondSelector,selectCharacter())
        .Times(1)
        .WillOnce(Return('c'));
    EXPECT_CALL(*secondSelector,getSeed())
        .Times(1)
        .WillOnce(Return(2));

    int firstId = 1;
    int secondId = 2;
    std::string firstQuery = "aa";
    MonkeyTyper firstTyper(firstId,std::move(firstSelector),firstQuery);
    firstTyper.moveStream(1);
    std::string secondQuery = "c";
    MonkeyTyper secondTyper(secondId,std::move(secondSelector),"c");

    two.insert(std::make_pair(firstId,std::move(firstTyper)));
    two.insert(std::make_pair(secondId,std::move(secondTyper)));

    unique_ptr<CounterIdMaker> idMaker = make_unique<CounterIdMaker>(100,std::set<int>());

    RingLeader test(two,std::move(idMaker));

    vector<ListInfo> expectedList;
    vector<char> firstPacketStream{'a'};
    vector<LetterOutcome> firstOutcomeStream{Match};
    vector<char> firstCorrespondingQuery{'a'};
    vector<int> firstPacketBestGuessLocation{1};
    ListInfo firstResult(1,1,1,1,firstPacketStream,firstOutcomeStream,firstCorrespondingQuery,firstPacketBestGuessLocation);
    ListInfo secondResult;
    secondResult.id = 2;

    expectedList.push_back(firstResult);
    expectedList.push_back(secondResult);

    StreamInfo expectedFirstStreamInfo(2,"a",firstResult);
    PromptInfo expectedFirstPromptInfo(2,firstQuery,firstResult);
    StreamInfo expectedSecondStreamInfo(2,"",secondResult);
    PromptInfo expectedSecondPromptInfo(2,secondQuery,secondResult);

    EXPECT_EQ(test.listInfo(),expectedList);
    EXPECT_EQ(test.streamInfo(1),expectedFirstStreamInfo);
    EXPECT_EQ(test.promptInfo(1),expectedFirstPromptInfo);
    EXPECT_EQ(test.streamInfo(2),expectedSecondStreamInfo);
    EXPECT_EQ(test.promptInfo(2),expectedSecondPromptInfo);

    std::vector<MonkeyTyperStatus> expectedStatus{MonkeyTyperStatus(1,PacketReady),MonkeyTyperStatus(2,Completed)};
    EXPECT_EQ(test.runNCharacters(1),expectedStatus);

    firstPacketStream = vector<char>{'b'};
    firstPacketBestGuessLocation = vector<int>{0};
    firstOutcomeStream = vector<LetterOutcome>{NoMatch};
    firstCorrespondingQuery = vector<char>{'a'};
    expectedList[0] = ListInfo(firstId,0,2,1,firstPacketStream,firstOutcomeStream,firstCorrespondingQuery,firstPacketBestGuessLocation);
    expectedFirstStreamInfo.stream = "ab";
    expectedFirstStreamInfo.listInfo = expectedList[0];
    expectedFirstPromptInfo.listInfo = expectedList[0];

    vector<char> secondPacketStream = vector<char>{'c'};
    vector<LetterOutcome> secondOutcomeStream = vector<LetterOutcome>{Complete};
    vector<char> secondCorrespondingQuery = vector<char>{'c'};
    vector<int> secondPacketBestGuessLocation = vector<int>{1};
    expectedList[1] = ListInfo(secondId,1,1,1,secondPacketStream,secondOutcomeStream,secondCorrespondingQuery,secondPacketBestGuessLocation);

    expectedSecondStreamInfo.stream = "c";
    expectedSecondStreamInfo.listInfo = expectedList[1];
    expectedSecondPromptInfo.listInfo = expectedList[1];

    EXPECT_EQ(test.listInfo(),expectedList);
    EXPECT_EQ(test.streamInfo(1),expectedFirstStreamInfo);
    EXPECT_EQ(test.promptInfo(1),expectedFirstPromptInfo);
    EXPECT_EQ(test.streamInfo(2),expectedSecondStreamInfo);
    EXPECT_EQ(test.promptInfo(2),expectedSecondPromptInfo);
}

TEST(RingLeaderTest,PauseTests){
    std::map<int,MonkeyTyper> two;
    
    std::string query1 = "abc";
    int id1 = 0;
    unique_ptr<MockLetterSelector> selector1 = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector1,getSeed())
        .Times(1)
        .WillOnce(Return(id1));
    EXPECT_CALL(*selector1,selectCharacter())
        .Times(3)
        .WillOnce(Return(query1[0]))
        .WillOnce(Return(query1[1]))
        .WillOnce(Return(query1[2]));
    
    std::string query2 = "cd";
    int id2 = 1;
    unique_ptr<MockLetterSelector> selector2 = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector2,getSeed())
        .Times(1)
        .WillOnce(Return(id2));
    EXPECT_CALL(*selector2,selectCharacter())
        .Times(3)
        .WillOnce(Return('c'))
        .WillOnce(Return('b'))
        .WillOnce(Return('d'));
    
    MonkeyTyper first(id1,std::move(selector1),query1);
    MonkeyTyper second(id2,std::move(selector2),query2);

    two.insert(std::make_pair(id1,std::move(first)));
    two.insert(std::make_pair(id2,std::move(second)));

    RingLeader test(two,nullptr);

    vector<char> stream1;
    vector<LetterOutcome> outcome1;
    vector<char> corresponding1;
    vector<int> location1;
    ListInfo expectedList1(id1,0,0,0,stream1,outcome1,corresponding1,location1);
    StreamInfo expectedStream1(id1,"",expectedList1);
    PromptInfo expectedPrompt1(id1,query1,expectedList1);

    vector<char> stream2;
    vector<LetterOutcome> outcome2;
    vector<char> corresponding2;
    vector<int> location2;
    ListInfo expectedList2(id2,0,0,0,stream2,outcome2,corresponding2,location2);
    StreamInfo expectedStream2(id2,"",expectedList2);
    PromptInfo expectedPrompt2(id2,query2,expectedList2);

    test.pauseMonkeyTyper(id1);
    test.unpauseMonkeyTyper(id2);

    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedList1,expectedList2}));
    EXPECT_EQ(test.streamInfo(id1),expectedStream1);
    EXPECT_EQ(test.promptInfo(id1),expectedPrompt1);
    EXPECT_EQ(test.streamInfo(id2),expectedStream2);
    EXPECT_EQ(test.promptInfo(id2),expectedPrompt2);


    //First Run. First MT is paused, second unpaused
    EXPECT_EQ(test.runNCharacters(1),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Paused),MonkeyTyperStatus(id2,PacketReady)}));

    stream2 = vector<char>{'c'};
    outcome2 = vector<LetterOutcome>{Match};
    corresponding2 = vector<char>{'c'};
    location2 = vector<int>{1};
    expectedList2 = ListInfo(id2,1,1,1,stream2,outcome2,corresponding2,location2);
    expectedPrompt2.listInfo = expectedList2;
    expectedStream2.listInfo = expectedList2;
    expectedStream2.stream = "c";
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedList1,expectedList2}));
    EXPECT_EQ(test.streamInfo(id1),expectedStream1);
    EXPECT_EQ(test.promptInfo(id1),expectedPrompt1);
    EXPECT_EQ(test.streamInfo(id2),expectedStream2);
    EXPECT_EQ(test.promptInfo(id2),expectedPrompt2);

    //second run. first MT unpaused, second paused.
    test.unpauseMonkeyTyper(id1);
    test.pauseMonkeyTyper(id2);

    EXPECT_EQ(test.runNCharacters(1),(vector<MonkeyTyperStatus>{MonkeyTyperStatus{id1,PacketReady},MonkeyTyperStatus{id2,Paused}}));

    stream1 = vector<char>{'a'};
    outcome1 = vector<LetterOutcome>{Match};
    corresponding1 = vector<char>{'a'};
    location1 = vector<int>{1};
    expectedList1 = ListInfo(id1,1,1,1,stream1,outcome1,corresponding1,location1);
    expectedPrompt1.listInfo = expectedList1;
    expectedStream1.listInfo = expectedList1;
    expectedStream1.stream = "a";
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedList1,expectedList2}));
    EXPECT_EQ(test.streamInfo(id1),expectedStream1);
    EXPECT_EQ(test.promptInfo(id1),expectedPrompt1);
    EXPECT_EQ(test.streamInfo(id2),expectedStream2);
    EXPECT_EQ(test.promptInfo(id2),expectedPrompt2);
    //Third "run". both MTs paused.
    test.pauseMonkeyTyper(id1);
    test.pauseMonkeyTyper(id2);
    
    EXPECT_EQ(test.runNCharacters(999),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Paused),MonkeyTyperStatus(id2,Paused)}));
    
    //Fourth run. both MTS unpaused.
    test.unpauseMonkeyTyper(id1);
    test.unpauseMonkeyTyper(id2);

    stream1 = vector<char>{'b','c'};
    outcome1 = vector<LetterOutcome>{Match,Complete};
    corresponding1 = vector<char>{'b','c'};
    location1 = vector<int>{2,3};
    expectedList1 = ListInfo(id1,3,3,3,stream1,outcome1,corresponding1,location1);
    expectedPrompt1.listInfo = expectedList1;
    expectedStream1.listInfo = expectedList1;
    expectedStream1.stream = "abc";

    expectedList2.currentLocation = 0;
    expectedList2.guessStreamSize = 3;
    expectedList2.promptRecord = 1;
    stream2 = vector<char>{'b','d'};
    outcome2 = vector<LetterOutcome>{NoMatch,NoMatch};
    corresponding2 = vector<char>{'d','c'};
    location2 = vector<int>{0,0};
    expectedList2 = ListInfo(id2,0,3,1,stream2,outcome2,corresponding2,location2);
    expectedPrompt2.listInfo = expectedList2;
    expectedStream2.listInfo = expectedList2;
    expectedStream2.stream = "cbd";

    EXPECT_EQ(test.runNCharacters(2),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Completed),MonkeyTyperStatus(id2,PacketReady)}));
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedList1,expectedList2}));
    EXPECT_EQ(test.promptInfo(id1),expectedPrompt1);
    EXPECT_EQ(test.streamInfo(id1),expectedStream1);
    EXPECT_EQ(test.promptInfo(id2),expectedPrompt2);
    EXPECT_EQ(test.streamInfo(id2),expectedStream2);

}

TEST(RingLeaderTest,CreateAndRemoveMonkeyTypers){
    int id1 = 10;
    int id2 = 20;
    int seed1 = 10;
    int seed2 = 20;
    std::string query1 = "ab";
    std::string query2 = "abcde";
    unique_ptr<MockLetterSelector> selector1 = make_unique<MockLetterSelector>();
    unique_ptr<MockLetterSelector> selector2 = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector1,getSeed())
        .Times(1)
        .WillOnce(Return(seed1));
    EXPECT_CALL(*selector1,selectCharacter())
        .Times(2)
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));

    EXPECT_CALL(*selector2,getSeed())
        .Times(1)
        .WillOnce(Return(seed2));
    EXPECT_CALL(*selector2,selectCharacter())
        .Times(3)
        .WillOnce(Return('a'))
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));
    
    unique_ptr<MockMonkeyTyperFactory> mockFactory = make_unique<MockMonkeyTyperFactory>();
    EXPECT_CALL(*mockFactory,build(id1,seed1,query1))
        .WillOnce(Return(MonkeyTyper(id1,std::move(selector1),query1)));
    EXPECT_CALL(*mockFactory,build(id2,seed2,query2))
        .WillOnce(Return(MonkeyTyper(id2,std::move(selector2),query2)));

    unique_ptr<MockIdMaker> mockId = make_unique<MockIdMaker>();
    EXPECT_CALL(*mockId,generateId())
        .Times(2)
        .WillOnce(Return(id1))
        .WillOnce(Return(id2));
    
    EXPECT_CALL(*mockId,releaseId(_))
        .Times(2);

    std::map<int,MonkeyTyper> empty;
    RingLeader test(empty,std::move(mockId),std::move(mockFactory));

    vector<char> expectedStream1;
    vector<LetterOutcome> expectedOutcome1;
    vector<char> expectedQuery1;
    vector<int> expectedLocation1;
    ListInfo expectedListInfo1{id1,0,0,0,expectedStream1,expectedOutcome1,expectedQuery1,expectedLocation1};
    StreamInfo expectedStreamInfo1(seed1,std::string(""),expectedListInfo1);
    PromptInfo expectedPromptInfo1(seed1,query1,expectedListInfo1);
    vector<char> expectedStream2;
    vector<LetterOutcome> expectedOutcome2;
    vector<char> expectedQuery2;
    vector<int> expectedLocation2;
    ListInfo expectedListInfo2(id2,0,0,0,expectedStream2,expectedOutcome2,expectedQuery2,expectedLocation2);
    StreamInfo expectedStreamInfo2(seed2,std::string(""),expectedListInfo2);
    PromptInfo expectedPromptInfo2(seed2,query2,expectedListInfo2);

    EXPECT_EQ(test.listInfo(),vector<ListInfo>());
    EXPECT_FALSE(test.promptInfo(id1));
    EXPECT_FALSE(test.streamInfo(id2));

    test.createMonkeyTyper(query1,seed1);
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo1}));
    EXPECT_EQ(test.runNCharacters(1),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,PacketReady)}));

    expectedStream1 = vector<char>{'a'};
    expectedOutcome1 = vector<LetterOutcome>{Match};
    expectedQuery1 = vector<char>{'a'};
    expectedLocation1 = vector<int>{1};
    expectedListInfo1 = ListInfo(id1,1,1,1,expectedStream1,expectedOutcome1,expectedQuery1,expectedLocation1);
    expectedPromptInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.stream = "a";
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo1}));
    EXPECT_EQ(test.promptInfo(id1),expectedPromptInfo1);
    EXPECT_EQ(test.streamInfo(id1),expectedStreamInfo1);

    test.createMonkeyTyper(query2,seed2);
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo1,expectedListInfo2}));
    EXPECT_EQ(test.runNCharacters(2),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Completed),MonkeyTyperStatus(id2,PacketReady)}));

    expectedStream1 = vector<char>{'b','a'};
    expectedOutcome1 = vector<LetterOutcome>{Complete,Untracked};
    expectedQuery1 = vector<char>{'b','a'};
    expectedLocation1 = vector<int>{2,0};
    expectedListInfo1 = ListInfo(id1,2,2,2,expectedStream1,expectedOutcome1,expectedQuery1,expectedLocation1);
    expectedPromptInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.stream = "ab";
    expectedStream2 = vector<char>{'a','a'};
    expectedOutcome2 = vector<LetterOutcome>{Match,Fallback};
    expectedQuery2 = vector<char>{'a','a'};
    expectedLocation2 = vector<int>{1,1};
    expectedListInfo2 = ListInfo(id2,1,2,1,expectedStream2,expectedOutcome2,expectedQuery2,expectedLocation2);
    expectedPromptInfo2.listInfo = expectedListInfo2;
    expectedStreamInfo2.listInfo = expectedListInfo2;
    expectedStreamInfo2.stream = "aa";

    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo1,expectedListInfo2}));
    EXPECT_EQ(test.promptInfo(id1),expectedPromptInfo1);
    EXPECT_EQ(test.streamInfo(id1),expectedStreamInfo1);
    EXPECT_EQ(test.promptInfo(id2),expectedPromptInfo2);
    EXPECT_EQ(test.streamInfo(id2),expectedStreamInfo2);
    test.removeMonkeyTyper(id1);
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo2}));
    EXPECT_FALSE(test.promptInfo(id1));
    EXPECT_FALSE(test.streamInfo(id1));

    EXPECT_EQ(test.runNCharacters(1),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id2,PacketReady)}));

    expectedStream2 = vector<char>{'b'};
    expectedOutcome2 = vector<LetterOutcome>{Match};
    expectedQuery2 = vector<char>{'b'};
    expectedLocation2 = vector<int>{2};
    expectedListInfo2 = ListInfo(id2,2,3,2,expectedStream2,expectedOutcome2,expectedQuery2,expectedLocation2);
    expectedPromptInfo2.listInfo = expectedListInfo2;
    expectedStreamInfo2.listInfo = expectedListInfo2;
    expectedStreamInfo2.stream = "aab";
    test.removeMonkeyTyper(id2);
    EXPECT_EQ(test.listInfo(),vector<ListInfo>());
    EXPECT_FALSE(test.promptInfo(id2));
    EXPECT_FALSE(test.streamInfo(id2));
    EXPECT_EQ(test.runNCharacters(999),vector<MonkeyTyperStatus>());
    test.removeMonkeyTyper(0);
    EXPECT_EQ(test.listInfo(),vector<ListInfo>());
    EXPECT_FALSE(test.promptInfo(id1));
    EXPECT_FALSE(test.streamInfo(id1));
    EXPECT_FALSE(test.promptInfo(id2));
    EXPECT_FALSE(test.streamInfo(id2));
    EXPECT_EQ(test.runNCharacters(999),vector<MonkeyTyperStatus>());
}

/**
 * This test uses the current classes with no mock values. 
 */
TEST(RingLeaderTest,NoMocks){
    unsigned int seed = 200;
    int startCounter = 399;
    int querySize = 94;
    mt19937LetterSelector control(default_alphabet,seed);
    std::string query = "";
    for(int i = 0; i < querySize; i++){
        query.push_back(control.selectCharacter());
    }
    
    unique_ptr<CounterIdMaker> counter = make_unique<CounterIdMaker>(startCounter,std::set<int>());
    unique_ptr<mt19937MonkeyTyperFactory> factory = make_unique<mt19937MonkeyTyperFactory>();
    std::map<int,MonkeyTyper> typers;
    RingLeader test(typers,std::move(counter),std::move(factory));

    test.createMonkeyTyper(query,seed);
    vector<char> expectedStream;
    vector<LetterOutcome> expectedOutcome;
    vector<char> expectedCorresponding;
    vector<int> expectedLocation;
    ListInfo expectedListInfo(startCounter,0,0,0,expectedStream,expectedOutcome,expectedCorresponding,expectedLocation);
    StreamInfo expectedStreamInfo(seed,std::string(""),expectedListInfo);
    PromptInfo expectedPromptInfo(seed,query,expectedListInfo);
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo}));
    EXPECT_EQ(test.promptInfo(startCounter),expectedPromptInfo);
    EXPECT_EQ(test.streamInfo(startCounter),expectedStreamInfo);
    EXPECT_EQ(test.runNCharacters(querySize),(std::vector<MonkeyTyperStatus>{MonkeyTyperStatus(startCounter,Completed)}));
    for(int i = 0; i < querySize; i++){
        expectedStream.push_back(query[i]);
        expectedCorresponding.push_back(query[i]);
        expectedLocation.push_back(i+1);
    }
    expectedOutcome = vector<LetterOutcome>(querySize-1,Match);
    expectedOutcome.push_back(Complete);
    expectedListInfo = ListInfo(startCounter,querySize,querySize,querySize,expectedStream,expectedOutcome,expectedCorresponding,expectedLocation);
    expectedStreamInfo.listInfo = expectedListInfo;
    expectedStreamInfo.stream = query;
    expectedPromptInfo.listInfo = expectedListInfo;
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo}));
    EXPECT_EQ(test.promptInfo(startCounter),expectedPromptInfo);
    EXPECT_EQ(test.streamInfo(startCounter),expectedStreamInfo);
}