#include "MonkeyTyperFactory.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"

#include <random>

#include "gtest/gtest.h"

using ::testing::Return;
using ::testing::Mock;

/**
 * Tests the mt19937MonkeyTyperFactory. Simply checks if the monkeyTyper is correctly set by checking its return values
 */
TEST(MonkeyTyperFactoryTest,mt19937Test){
    unsigned int seed = 100;
    std::string testQuery = "This should be larger than the query to prevent issues.";
    int runs = testQuery.size()-1;
    int id = 10;
    unique_ptr<mt19937LetterSelector> controlSelector = make_unique<mt19937LetterSelector>(default_alphabet,seed);
    MonkeyTyper control(id,std::move(controlSelector),testQuery);
    
    mt19937MonkeyTyperFactory factory;
    MonkeyTyper test = factory.build(id,seed,testQuery);

    control.moveStream(runs);
    test.moveStream(runs);

    EXPECT_EQ(control.typerInfo(),test.typerInfo());
}

/**
 * Tests the MockMonkeyTyperFactory
 */
TEST(MonkeyTyperFactoryTest,MockTest){
    int id = 125;
    int seed = 25;
    std::string query = "ab";

    unique_ptr<MockLetterSelector> mockSelector = make_unique<MockLetterSelector>();
    EXPECT_CALL(*mockSelector,selectCharacter())
        .Times(2)
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));
    EXPECT_CALL(*mockSelector,getSeed())
        .Times(1)
        .WillOnce(Return(seed));

    MockMonkeyTyperFactory mockFactory;
    EXPECT_CALL(mockFactory,build(id,seed,query))
        .Times(1)
        .WillOnce(Return(MonkeyTyper(id,std::move(mockSelector),query)));

    MonkeyTyper test = mockFactory.build(id,seed,query);
    
    EXPECT_EQ(Completed,test.moveStream(2));
    vector<char> expectedPacketStream{'a','b'};
    vector<LetterOutcome> expectedOutcome{Match,Complete};
    vector<char> expectedCorrespondingQuery{'a','b'};
    vector<int> expectedBestLocation{1,2};
    ListInfo expectedInfo(id,2,2,2,expectedPacketStream,expectedOutcome,expectedCorrespondingQuery,expectedBestLocation);
    TyperInfo expectedTyperInfo(expectedInfo,std::string("ab"),query,seed);
    EXPECT_EQ(test.listInfo(),expectedInfo);
    EXPECT_EQ(test.typerInfo(),expectedTyperInfo);
}