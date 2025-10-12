#include "RingLeader.hpp"

#include <memory>
#include <gtest/gtest.h>


TEST(RingLeaderSingularInfoTest,EmptyList){

}

TEST(RingLeaderSingularInfoTest,IdMiss){

}

TEST(RingLeaderSingularInfoTest,OneMonkeyTyper){

}

TEST(RingLeaderSingularInfoTest,TwoMonkeysOneUnrun){

}

TEST(RingLeaderListInfoTest,EmptyList){
    std::map<int,MonkeyTyper> empty;
    std::set<int> used_ids;
    unique_ptr<CounterIdMaker> idGenerator = make_unique<CounterIdMaker>(50,used_ids);
    RingLeader test(empty,std::move(idGenerator));
    std::vector<ListInfo> expectedResult;
    EXPECT_EQ(test.listInfo(),expectedResult);
}

TEST(RingLeaderListInfoTest,OneMonkeyTyper){

}

TEST(RingLeaderListInfoTest,TwoMonkeysOneUnrun){

}

