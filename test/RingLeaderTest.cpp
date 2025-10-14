#include "RingLeader.hpp"
#include "mockClasses.hpp"

#include <memory>
#include <gtest/gtest.h>
TEST(RingLeaderWholisticTest,EmptyList){
    std::map<int,MonkeyTyper> empty;
    std::set<int> used_ids;
    unique_ptr<CounterIdMaker> idGenerator = make_unique<CounterIdMaker>(50,used_ids);
    RingLeader test(empty,std::move(idGenerator));
    std::vector<ListInfo> expectedResult;
    EXPECT_EQ(test.listInfo(),expectedResult);
}

TEST(RingLeaderWholisticTest,OneMonkeyTyper){

}

TEST(RingLeaderWholisticTest,TwoMonkeysOneUnrun){

}

