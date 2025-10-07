#include "RingLeader.hpp"

#include <gtest/gtest.h>

/**
 * CounterIdGeneratorTests
 */

void counterIdTestHelper(int starterCounter, int expectedId, int expectedNextId, std::set<int>&starterSet, std::set<int>&endSet){
    CounterIdMaker test(starterCounter,starterSet);

    EXPECT_EQ(test.getCounter(),starterCounter);
    EXPECT_EQ(test.getUsedIds(),starterSet);

    EXPECT_EQ(test.generateId(),expectedId);

    EXPECT_EQ(test.getCounter(),expectedNextId);
    EXPECT_EQ(test.getUsedIds(),endSet);
}

TEST(CounterIdGenerateId,EmptyUsedIds){
    int starterCounter = 100;
    int expectedId = 100;
    int expectedNextId = 101;
    std::set<int> starterSet;
    std::set<int> endSet{100};
    counterIdTestHelper(starterCounter,expectedId,expectedNextId,starterSet,endSet);
}

TEST(CounterIdGenerateId,NoCollision){
    int starterCounter = 95;
    int expectedId = 95;
    int expectedNextId = 96;
    std::set<int> starterSet{94,96};
    std::set<int> endSet{94,95,96};
    counterIdTestHelper(starterCounter,expectedId,expectedNextId,starterSet,endSet);
}

TEST(CounterIdGenerateId,CollisionSkipOne){
    int starterCounter = 5;
    int expectedId = 6;
    int expectedNextId = 7;
    std::set<int> starterSet{5,7};
    std::set<int> endSet{5,6,7};
    counterIdTestHelper(starterCounter,expectedId,expectedNextId,starterSet,endSet);
}

TEST(CounterIdGenerateId,CollisionSkipTwo){
    int starterCounter = 1;
    int expectedId = 3;
    int expectedNextId = 4;
    std::set<int> starterSet{1,2,4};
    std::set<int> endSet{1,2,3,4};
    counterIdTestHelper(starterCounter,expectedId,expectedNextId,starterSet,endSet);
}