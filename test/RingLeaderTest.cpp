#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"

#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;

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
    
}

TEST(RingLeaderTest,CreateAndRemoveMonkeyTypers){

}