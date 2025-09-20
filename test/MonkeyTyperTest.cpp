#include "mockClasses.hpp"
#include "MonkeyTyper.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;

template <class T>
bool checkQueueEquality(queue<T> first, queue<T> second){
    vector<T> firstHolder;
    while(first.size() != 0){
        firstHolder.push_back(first.front());
        first.pop();
    }
    vector<T> secondHolder;
    while(second.size() != 0){
        secondHolder.push_back(second.front());
        second.pop();
    }

    sort(firstHolder.begin(),firstHolder.end());
    sort(secondHolder.begin(),secondHolder.end());
    return firstHolder == secondHolder;
}

void positionHolderTestHelper(PositionHolder &test, char selection, int expectedPromptRecord, int expectedHighestSpot, queue<int> expectedCurrSpot,bool expectedComplete, string testName=""){
    test.evaluateSelection(selection);

    EXPECT_EQ(test.getHighestSpot(),expectedHighestSpot) << "Issue with the highest spot determined. Test " << testName;
    EXPECT_EQ(test.promptRecord, expectedPromptRecord) << "Issue with the promptRecord Test " << testName;
    if(!expectedComplete)
        EXPECT_TRUE(checkQueueEquality(test.currentSpot,expectedCurrSpot)) << "There is an incorrect spot indicated in currSpot. Test " << testName;
    EXPECT_EQ(test.complete,expectedComplete) << "The complete flag is incorrectly set. Test " << testName;
}

TEST(PositionHolderEvalSelectionTest,EmptyCurrSpotCheck){
    queue<int> initialCurrSpot;
    PositionHolder test("b",initialCurrSpot,0);
    queue<int> expected;
    positionHolderTestHelper(test,'a',0,0,expected,false);

    queue<int> expectedResult;
    expectedResult.push(1);
    positionHolderTestHelper(test,'b',1,1,expectedResult,true);
}

TEST(PositionHolderEvalSelectionTest,SingleEntryInCurrSpot){
    queue<int> singular;
    singular.push(1);
    PositionHolder bothFail("ab",singular,1);
    queue<int> empty;
    positionHolderTestHelper(bothFail,'c',1,0,empty,false,"Both characters incorrect.");

    PositionHolder firstCharCorrect("ab",singular,1);
    positionHolderTestHelper(firstCharCorrect,'a',1,1,queue<int>(singular),false,"Only first query char correct.");

    queue<int> expect2;
    expect2.push(2);
    PositionHolder secondCharCorrect("ab",singular,1);
    positionHolderTestHelper(secondCharCorrect,'b',2,2,queue<int>(expect2),true,"Only second query char correct.");

    queue<int> expectBoth(expect2);
    expectBoth.push(1);
    PositionHolder bothCorrect("aa",singular,1);
    positionHolderTestHelper(bothCorrect,'a',2,2,expectBoth,true, "Both chars match.");
}

TEST(PositionHolderEvalSelectionTest,PromptRecordTest){
    queue<int> startingQueue;
    startingQueue.push(2);
    PositionHolder test("abcd",startingQueue,2);

    queue<int> expectedFirstCurrSpot;
    expectedFirstCurrSpot.push(1);
    positionHolderTestHelper(test,'a',2,1,expectedFirstCurrSpot,false,"First test");

    queue<int> secondTest;
    secondTest.push(2);
    positionHolderTestHelper(test,'b',2,2,secondTest,false,"Second test");

    queue<int> thirdTest;
    thirdTest.push(3);
    positionHolderTestHelper(test,'c',3,3,thirdTest,false,"Third test");

    queue<int> fourthTest;
    fourthTest.push(1);
    positionHolderTestHelper(test,'a',3,1,fourthTest,false,"Fourth test");
}

void monkeyTyperEvalSelectionTestHelper(MonkeyTyper &test,int expectedStreamSize, int expectedPromptRecord, string expectedGuessStream, bool expectedCompleteFlag, std::string message){
    EXPECT_EQ(test.getPromptRecord(),expectedPromptRecord) << "Prompt Record Mismatch in case " << message;
    EXPECT_EQ(test.getGuessString(),expectedGuessStream) << "Guess Stream Mismatch in case " << message;
    EXPECT_EQ(test.getTotalStreamSize(),expectedStreamSize) <<"Stream Size Mismatch in case " << message;
    EXPECT_EQ(test.complete(),expectedCompleteFlag) << "Completion Flag Mismatch in case " << message;
}

TEST(MonkeyTyperPositionHolderIntegration,TestCorrectSelections){
    MonkeyTyper test(0, nullptr, "ab");
    monkeyTyperEvalSelectionTestHelper(test,0,0,"",false, "Case Monkey Typer initialization");

    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,1,1,"a",false, "Case First Char Correct");

    EXPECT_EQ(test.evaluateSelection('b'),2);
    monkeyTyperEvalSelectionTestHelper(test,2,2,"ab",true, "Case Second Char Correct");
}

TEST(MonkeyTyperPositionHolderIntegration,IncorrectThenCorrect){
    queue<int> testCurrSpot;
    testCurrSpot.push(1);
    PositionHolder hold("ab",testCurrSpot,1);
    MonkeyTyper test(0,nullptr,hold,"ab",0);
    monkeyTyperEvalSelectionTestHelper(test,0,1,"",false, "Case Initialization with PositionHolder defined.");

    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,1,1,"a",false, "Case Incorrect Char Evaluated");

    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,2,1,"aa",false, "Case Correct Char Evaluated");
}

TEST(MonkeyTyperPositionHolderIntegration,SingleCharCorrect){
    MonkeyTyper test(0,nullptr,"a");
    monkeyTyperEvalSelectionTestHelper(test,0,0,"",false, "Case Initialization");
    
    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,1,1,"a",true, "Case Correct Single Char Evaluated");
}

void moveStreamTestHelper(MonkeyTyper &test, int size, vector<char> &expectedStream, vector<LetterOutcome> &expectedCorrectness, vector<int> &expectedLocation,
    vector<char> expectedQuery, Status expectedStatus){

    EXPECT_EQ(expectedStatus,test.moveStream(size));
    ListInfo resultListInfo = test.listInfo();
    EXPECT_EQ(expectedStream,resultListInfo.packetStream);
    EXPECT_EQ(expectedLocation,resultListInfo.packetBestGuessLocation);
    EXPECT_EQ(expectedCorrectness,resultListInfo.packetCorrectness);
    EXPECT_EQ(expectedQuery,resultListInfo.packetCorrespondingQuery);
}

TEST(MonkeyTyperStreamTest,SingleStream){
    
    MockLetterSelector mockSelector;
    std::string query = "xyz";
    int size = 7;
    vector<char> expectedStream{'x','z','x','y','x','y','z'};
    vector<LetterOutcome> expectedCorrectness{Match,NoMatch,Match,Match,Fallback,Match,Complete};
    vector<int> expectedLocation{1,0,1,2,1,2,3};
    vector<char> expectedQuery{'x','y','x','y','x','y','z'};
    Status expectedStatus = Completed;

    EXPECT_CALL(mockSelector,selectCharacter())
        .Times(7)
        .WillOnce(Return('x'))
        .WillOnce(Return('z'))
        .WillOnce(Return('x'))
        .WillOnce(Return('y'))
        .WillOnce(Return('x'))
        .WillOnce(Return('y'))
        .WillOnce(Return('z'));

    MonkeyTyper test(0,&mockSelector,query);
    
    moveStreamTestHelper(test, size, expectedStream, expectedCorrectness, expectedLocation, expectedQuery, expectedStatus);
}
int main(int argc, char **argv) {
    cout << "HELLO";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
