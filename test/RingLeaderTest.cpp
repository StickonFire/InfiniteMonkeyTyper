#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"

#include <memory>
#include <gtest/gtest.h>
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

}

TEST(RingLeaderWholisticTest,TwoMonkeysOneUnrun){

}

