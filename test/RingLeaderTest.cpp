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

void releaseIdHelper(int starterCounter, int idToRemove, int expectedId, std::set<int> &starterUsedIds, std::set<int> &afterCallUsedIds){
    CounterIdMaker test(starterCounter,starterUsedIds);
    test.releaseId(idToRemove);
    EXPECT_EQ(test.getUsedIds(),afterCallUsedIds);
    EXPECT_EQ(test.generateId(),expectedId);
}

TEST(CounterIdReleaseId,Empty){
    int starterCounter = 0;
    int idToRemove = 0;
    int expectedId = 0;
    std::set<int> starterUsedIds;
    std::set<int> afterCallUsedIds;
    releaseIdHelper(starterCounter, idToRemove, expectedId, starterUsedIds, afterCallUsedIds);
}

TEST(CounterIdReleaseId,Missed){
    int starterCounter = 125;
    int idToRemove = 127;
    int expectedId = 126;
    std::set<int> starterUsedIds{125};
    std::set<int> afterCallUsedIds{125};
    releaseIdHelper(starterCounter, idToRemove, expectedId, starterUsedIds, afterCallUsedIds);
}

TEST(CounterIdReleaseId,Hit){
    int starterCounter = 170;
    int idToRemove = 170;
    int expectedId = 170;
    std::set<int> starterUsedIds{169,170,171};
    std::set<int> afterCallUsedIds{169,171};
    releaseIdHelper(starterCounter, idToRemove, expectedId, starterUsedIds, afterCallUsedIds);
}