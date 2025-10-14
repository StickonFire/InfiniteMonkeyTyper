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
}

TEST(RingLeaderWholisticTest,OneMonkeyTyper){

}

TEST(RingLeaderWholisticTest,TwoMonkeysOneUnrun){

}

