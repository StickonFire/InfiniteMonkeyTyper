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

class TyperInfoEqualityTestSuite : public testing::Test {

    protected:
        TyperInfoEqualityTestSuite(): empty(), first(empty,"95","65",40), second(empty,"95","65",40){ }
        ListInfo empty;
        TyperInfo first;
        TyperInfo second;
};

TEST_F(TyperInfoEqualityTestSuite,Correct){
    EXPECT_EQ(first,second);
}

TEST_F(TyperInfoEqualityTestSuite,CycleThroughIncorrect){
    std::vector<char> packetStream{'a'};
    std::vector<LetterOutcome> packetOutcome{Match};
    std::vector<char> packetCorresponding{'a'};
    std::vector<int> packetLocation{1};
    ListInfo different(10,10,10,10,packetStream,packetOutcome,packetCorresponding,packetLocation);
    first.listInfo = ListInfo(different);
    EXPECT_FALSE(first == second) << "TyperInfo ListInfo different but incorrectly marked equal";
    second.listInfo = ListInfo(different);
    EXPECT_EQ(first,second) << "TyperInfo ListInfos are equal, but incorrectly marked unequal";

    std::string replacementPrompt = "replacementPrompt";
    first.prompt = replacementPrompt;
    EXPECT_FALSE(first == second) << "TyperInfo prompts are unequal, but declared equal";
    second.prompt = replacementPrompt;
    EXPECT_EQ(first,second) << "TyperInfo prompts are equal, but declared unequal";

    std::string replacementStream = "replacementStream";
    first.stream = replacementStream;
    EXPECT_FALSE(first == second) << "TyperInfo streams are unequal, but declared equal";
    second.stream = replacementStream;
    EXPECT_EQ(first,second) << "TyperInfo streams are equal, but declared unequal";

    unsigned int replacementSeed = 34;
    first.seed = replacementSeed;
    EXPECT_FALSE(first == second) << "TyperInfo seeds are unequal, but declared equal";
    second.seed = replacementSeed;
    EXPECT_EQ(first,second) << "TyperInfo seeds are equal, but declared unequal";
}
