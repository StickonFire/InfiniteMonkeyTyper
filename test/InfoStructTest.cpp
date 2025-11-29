#include "ModelInfo.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <vector>
#include <iostream>

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
        ListInfo empty;
        TyperInfo first;
        TyperInfo second;

        void checkInfoInequality(std::string message){
            EXPECT_FALSE(first == second) << message << "First: " << first << "\nSecond: " << second;
        }

        void checkInfoEquality(std::string message){
            EXPECT_EQ(first,second) << message << "First: " << first << "\nSecond: " << second;
        }

        TyperInfoEqualityTestSuite(): empty(), first(empty,"95","65",40), second(empty,"95","65",40){ }
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
    checkInfoInequality("TyperInfo's ListInfo are unequal, but found to be equal.");
    second.listInfo = ListInfo(different);
    checkInfoEquality("TyperInfo's ListInfo are equal, but found unequal.");

    std::string replacementPrompt = "replacementPrompt";
    first.prompt = replacementPrompt;
    checkInfoInequality("TyperInfo prompts are unequal, but declared equal");
    second.prompt = replacementPrompt;
    checkInfoEquality("TyperInfo prompts are equal, but declared unequal");

    std::string replacementStream = "replacementStream";
    first.stream = replacementStream;
    checkInfoInequality("TyperInfo streams are unequal, but declared equal");
    second.stream = replacementStream;
    checkInfoEquality("TyperInfo streams are equal, but declared unequal");

    unsigned int replacementSeed = 34;
    first.seed = replacementSeed;
    checkInfoInequality("TyperInfo seeds are unequal, but declared equal");
    second.seed = replacementSeed;
    checkInfoEquality("TyperInfo seeds are equal, but declared unequal");
}

class ModelInfoTestSuite: public testing::Test {
    protected:
        std::map<int,std::string> emptyMessages;
        std::map<int,TyperInfo> emptyTypers;
        ModelInfo first;
        ModelInfo second;

        void checkIfTrue(bool isTrue,std::string message){
            EXPECT_TRUE(isTrue) << message << "First: " << first << "\nSecond: " << second;
        }

        ModelInfoTestSuite(): emptyMessages(), emptyTypers(), first(emptyMessages,emptyTypers), 
                                second(emptyMessages,emptyTypers){ }
};

TEST_F(ModelInfoTestSuite,Correct){
    EXPECT_EQ(first,second) << "These ModelInfos are equal, but are labelled unequal.";
}

TEST_F(ModelInfoTestSuite,CycleThroughIncorrect){
    int messageId = 0;
    std::string message = "First";
    int typerId = 10;
    ListInfo empty;
    TyperInfo addedTyper(empty,"A","B",0);
    first.messages[messageId] = message;
    checkIfTrue(!(first == second),"ModelInfo equality operator failed to notice difference in message.");
    second.messages[messageId] = message;
    checkIfTrue(first == second,"ModelInfo equality operator fails to notice the two are equal after equalizing messages");

    first.typerValues.insert(std::make_pair(typerId,addedTyper));
    checkIfTrue(!(first == second),"ModelInfo equality operator failed to notice difference in typerValues.");
    second.typerValues.insert(std::make_pair(typerId,addedTyper));
    checkIfTrue(first == second,"ModelInfo equality operator fails to notice the two are equal after equalizing typerInfos");
}