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

}

