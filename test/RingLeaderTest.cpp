#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"

#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
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

/**
 * Wholistic tests for the following functions:
 *  - Constructor
 *  - runNCharacters
 *  - listInfo
 *  - promptInfo
 *  - streamInfo
 */
TEST(RingLeaderWholisticTest,EmptyList){
    std::map<int,MonkeyTyper> empty;
    std::set<int> usedIds;
    unique_ptr<CounterIdMaker> idGenerator = make_unique<CounterIdMaker>(50,usedIds);
    RingLeader test(empty,std::move(idGenerator));
    std::vector<ListInfo> expectedResult;
    EXPECT_EQ(test.listInfo(),expectedResult);
    EXPECT_FALSE(test.promptInfo(1));
    EXPECT_FALSE(test.streamInfo(3));

    vector<MonkeyTyperStatus> expected;
    EXPECT_EQ(test.runNCharacters(100),expected);

    EXPECT_EQ(test.listInfo(),expectedResult);
    EXPECT_FALSE(test.promptInfo(1));
    EXPECT_FALSE(test.streamInfo(3));
}

TEST(RingLeaderWholisticTest,OneMonkeyTyper){
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

TEST(RingLeaderWholisticTest,TwoMonkeysOneUnrun){
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

    expectedList[0].currentLocation = 0;
    expectedList[0].guessStreamSize = 2;
    expectedList[0].promptRecord = 1;
    expectedList[0].packetStream = vector<char>{'b'};
    expectedList[0].packetBestGuessLocation = vector<int>{0};
    expectedList[0].packetCorrectness = vector<LetterOutcome>{NoMatch};
    expectedList[0].packetCorrespondingQuery = vector<char>{'a'};

    expectedFirstStreamInfo.stream = "ab";
    expectedFirstStreamInfo.listInfo = expectedList[0];
    expectedFirstPromptInfo.listInfo = expectedList[0];

    expectedList[1].currentLocation = 1;
    expectedList[1].guessStreamSize = 1;
    expectedList[1].promptRecord = 1;
    expectedList[1].packetStream = vector<char>{'c'};
    expectedList[1].packetCorrectness = vector<LetterOutcome>{Complete};
    expectedList[1].packetCorrespondingQuery = vector<char>{'c'};
    expectedList[1].packetBestGuessLocation = vector<int>{1};

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

    expectedList2.currentLocation = 1;
    expectedList2.promptRecord = 1;
    expectedList2.guessStreamSize = 1;
    expectedList2.packetStream = vector<char>{'c'};
    expectedList2.packetCorrectness = vector<LetterOutcome>{Match};
    expectedList2.packetCorrespondingQuery = vector<char>{'c'};
    expectedList2.packetBestGuessLocation = vector<int>{1};
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

    expectedList1.currentLocation = 1;
    expectedList1.guessStreamSize = 1;
    expectedList1.promptRecord = 1;
    expectedList1.packetStream = vector<char>{'a'};
    expectedList1.packetCorrectness = vector<LetterOutcome>{Match};
    expectedList1.packetCorrespondingQuery = vector<char>{'a'};
    expectedList1.packetBestGuessLocation = vector<int>{1};
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

    expectedList1.currentLocation = 3;
    expectedList1.guessStreamSize = 3;
    expectedList1.promptRecord = 3;
    expectedList1.packetStream = vector<char>{'b','c'};
    expectedList1.packetCorrectness = vector<LetterOutcome>{Match,Complete};
    expectedList1.packetCorrespondingQuery = vector<char>{'b','c'};
    expectedList1.packetBestGuessLocation = vector<int>{2,3};
    expectedPrompt1.listInfo = expectedList1;
    expectedStream1.listInfo = expectedList1;
    expectedStream1.stream = "abc";

    expectedList2.currentLocation = 0;
    expectedList2.guessStreamSize = 3;
    expectedList2.promptRecord = 1;
    expectedList2.packetStream = vector<char>{'b','d'};
    expectedList2.packetCorrectness = vector<LetterOutcome>{NoMatch,NoMatch};
    expectedList2.packetCorrespondingQuery = vector<char>{'d','c'};
    expectedList2.packetBestGuessLocation = vector<int>{0,0};
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
    expectedListInfo1.currentLocation = 1;
    expectedListInfo1.guessStreamSize = 1;
    expectedListInfo1.promptRecord = 1;
    expectedListInfo1.packetStream = vector<char>{'a'};
    expectedListInfo1.packetCorrectness = vector<LetterOutcome>{Match};
    expectedListInfo1.packetCorrespondingQuery = vector<char>{'a'};
    expectedListInfo1.packetBestGuessLocation = vector<int>{1};
    expectedPromptInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.stream = "a";
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo1}));
    EXPECT_EQ(test.promptInfo(id1),expectedPromptInfo1);
    EXPECT_EQ(test.streamInfo(id1),expectedStreamInfo1);

    test.createMonkeyTyper(query2,seed2);
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo1,expectedListInfo2}));
    EXPECT_EQ(test.runNCharacters(2),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Completed),MonkeyTyperStatus(id2,PacketReady)}));

    expectedListInfo1.currentLocation = 2;
    expectedListInfo1.guessStreamSize = 2;
    expectedListInfo1.promptRecord = 2;
    expectedListInfo1.packetStream = vector<char>{'b','a'};
    expectedListInfo1.packetCorrectness = vector<LetterOutcome>{Complete,Untracked};
    expectedListInfo1.packetCorrespondingQuery = vector<char>{'b','a'};
    expectedListInfo1.packetBestGuessLocation = vector<int>{2,0};
    expectedPromptInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.listInfo = expectedListInfo1;
    expectedStreamInfo1.stream = "ab";
    expectedListInfo2.currentLocation = 1;
    expectedListInfo2.guessStreamSize = 2;
    expectedListInfo2.promptRecord = 1;
    expectedListInfo2.packetStream = vector<char>{'a','a'};
    expectedListInfo2.packetCorrectness = vector<LetterOutcome>{Match,Fallback};
    expectedListInfo2.packetCorrespondingQuery = vector<char>{'a','a'};
    expectedListInfo2.packetBestGuessLocation = vector<int>{1,1};
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
    expectedListInfo2.currentLocation = 2;
    expectedListInfo2.guessStreamSize = 3;
    expectedListInfo2.promptRecord = 2;
    expectedListInfo2.packetStream = vector<char>{'b'};
    expectedListInfo2.packetCorrectness = vector<LetterOutcome>{Match};
    expectedListInfo2.packetCorrespondingQuery = vector<char>{'b'};
    expectedListInfo2.packetBestGuessLocation = vector<int>{2};
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