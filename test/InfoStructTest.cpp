#include "ModelInfo.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <vector>

using ::testing::Return;
using ::testing::Mock;
using ::testing::Test;

TEST(ListInfoEquality,Correct){
    std::vector<char> packetStream;
    std::vector<LetterOutcome> packetCorrectness;
    std::vector<char> packetCorrespondingQuery;
    std::vector<int> packetBestGuessLocation;
    ListInfo first(0,1,2,3,packetStream,packetCorrectness,packetCorrespondingQuery,packetBestGuessLocation);
    ListInfo second(0,1,2,3,packetStream,packetCorrectness,packetCorrespondingQuery,packetBestGuessLocation);
    EXPECT_TRUE(first == second);
}

TEST(ListInfoEquality,Defaults){
    ListInfo first;
    ListInfo second;
    EXPECT_TRUE(first == second);
}

TEST(ListInfoEquality,CycleThroughIncorrect){
    std::vector<char> packetStream;
    std::vector<LetterOutcome> packetCorrectness;
    std::vector<char> packetCorrespondingQuery;
    std::vector<int> packetBestGuessLocation;
    ListInfo first(0,1,2,3,packetStream,packetCorrectness,packetCorrespondingQuery,packetBestGuessLocation);
    ListInfo second(1,1,2,3,packetStream,packetCorrectness,packetCorrespondingQuery,packetBestGuessLocation);
    EXPECT_FALSE(first == second);

    first.id = second.id;
    EXPECT_TRUE(first == second);
    second.currentLocation++;
    EXPECT_FALSE(first == second);

    first.currentLocation = second.currentLocation;
    EXPECT_TRUE(first == second);
    second.guessStreamSize++;
    EXPECT_FALSE(first == second);

    first.guessStreamSize = second.guessStreamSize;
    EXPECT_TRUE(first == second);
    second.promptRecord++;
    EXPECT_FALSE(first == second);

    first.promptRecord = second.promptRecord;
    EXPECT_TRUE(first == second);
    second.packetStream.push_back('a');
    EXPECT_FALSE(first == second);

    first.packetStream = second.packetStream;
    EXPECT_TRUE(first == second);
    second.packetCorrectness.push_back(Complete);
    EXPECT_FALSE(first == second);

    first.packetCorrectness = second.packetCorrectness;
    EXPECT_TRUE(first == second);
    second.packetCorrespondingQuery.push_back('a');
    EXPECT_FALSE(first == second);

    first.packetCorrespondingQuery = second.packetCorrespondingQuery;
    EXPECT_TRUE(first == second);
    second.packetBestGuessLocation.push_back(1);
    EXPECT_FALSE(first == second);
}
