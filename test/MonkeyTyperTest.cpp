#include "mockClasses.hpp"
#include "MonkeyTyper.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using ::testing::Return;
using ::testing::Mock;

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
    unique_ptr<MockLetterSelector> onlySeed = make_unique<MockLetterSelector>();
    EXPECT_CALL(*onlySeed,getSeed())
        .WillOnce(Return(0));
    MonkeyTyper test(0, std::move(onlySeed), "ab");
    monkeyTyperEvalSelectionTestHelper(test,0,0,"",false, "Case Monkey Typer initialization");

    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,1,1,"a",false, "Case First Char Correct");

    EXPECT_EQ(test.evaluateSelection('b'),2);
    monkeyTyperEvalSelectionTestHelper(test,2,2,"ab",true, "Case Second Char Correct");
}

TEST(MonkeyTyperPositionHolderIntegration,IncorrectThenCorrect){
    unique_ptr<MockLetterSelector> onlySeed = make_unique<MockLetterSelector>();
    EXPECT_CALL(*onlySeed,getSeed())
        .WillOnce(Return(0));

    queue<int> testCurrSpot;
    testCurrSpot.push(1);
    PositionHolder hold("ab",testCurrSpot,1);
    MonkeyTyper test(0,std::move(onlySeed),hold,"ab",0);
    monkeyTyperEvalSelectionTestHelper(test,0,1,"",false, "Case Initialization with PositionHolder defined.");

    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,1,1,"a",false, "Case Incorrect Char Evaluated");

    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,2,1,"aa",false, "Case Correct Char Evaluated");
}

TEST(MonkeyTyperPositionHolderIntegration,SingleCharCorrect){
    unique_ptr<MockLetterSelector> onlySeed = make_unique<MockLetterSelector>();
    EXPECT_CALL(*onlySeed,getSeed())
        .WillOnce(Return(0));
    MonkeyTyper test(0,std::move(onlySeed),"a");
    monkeyTyperEvalSelectionTestHelper(test,0,0,"",false, "Case Initialization");
    
    EXPECT_EQ(test.evaluateSelection('a'),1);
    monkeyTyperEvalSelectionTestHelper(test,1,1,"a",true, "Case Correct Single Char Evaluated");
}

void moveStreamTestHelper(MonkeyTyper &test, int size, vector<char> &expectedStream, vector<LetterOutcome> &expectedCorrectness, vector<int> &expectedLocation,
    vector<char> expectedQuery, Status expectedStatus, std::string query, std::string stream, int seed){

    EXPECT_EQ(expectedStatus,test.moveStream(size));
    ListInfo resultListInfo = test.listInfo();
    EXPECT_EQ(expectedStream,resultListInfo.packetStream);
    EXPECT_EQ(expectedLocation,resultListInfo.packetBestGuessLocation);
    EXPECT_EQ(expectedCorrectness,resultListInfo.packetCorrectness);
    EXPECT_EQ(expectedQuery,resultListInfo.packetCorrespondingQuery);
    TyperInfo expectedTyperInfo(resultListInfo,stream,query,seed);
    EXPECT_EQ(test.typerInfo(),expectedTyperInfo);
}

TEST(MonkeyTyperStreamTest,SingleStream){
    
    unique_ptr<MockLetterSelector> mockSelector = make_unique<MockLetterSelector>();
    std::string query = "xyz";
    int size = 7;
    std::string stream = "xzxyxyz";
    vector<char> expectedStream(stream.begin(),stream.end());
    vector<LetterOutcome> expectedCorrectness{Match,NoMatch,Match,Match,Fallback,Match,Complete};
    vector<int> expectedLocation{1,0,1,2,1,2,3};
    vector<char> expectedQuery{'x','y','x','y','x','y','z'};
    Status expectedStatus = Completed;

    EXPECT_CALL(*mockSelector,selectCharacter())
        .Times(7)
        .WillOnce(Return('x'))
        .WillOnce(Return('z'))
        .WillOnce(Return('x'))
        .WillOnce(Return('y'))
        .WillOnce(Return('x'))
        .WillOnce(Return('y'))
        .WillOnce(Return('z'));
    EXPECT_CALL(*mockSelector,getSeed())
        .Times(1)
        .WillOnce(Return(0));

    MonkeyTyper test(0,std::move(mockSelector),query);
    
    moveStreamTestHelper(test, size, expectedStream, expectedCorrectness, expectedLocation, expectedQuery, expectedStatus,query,stream,0);
}

TEST(MonkeyTyperStreamTest,DoubleStream){
    
    unique_ptr<MockLetterSelector> mockSelector = make_unique<MockLetterSelector>();
    std::string query = "pqrstu";
    int size = 6;
    std::string stream1 = "pqtpqp";
    vector<char> expectedStream1(stream1.begin(),stream1.end());
    vector<LetterOutcome> expectedCorrectness1{Match,Match,NoMatch,Match,Match,Fallback};
    vector<int> expectedLocation1{1,2,0,1,2,1};
    vector<char> expectedQuery1{'p','q','r','p','q','p'};
    Status expectedStatus1 = PacketReady;

    std::string stream2 = "qrstu";
    vector<char> expectedStream2(stream2.begin(),stream2.end());
    expectedStream2.push_back('a');
    vector<LetterOutcome> expectedCorrectness2{Match,Match,Match,Match,Complete,Untracked};
    vector<int> expectedLocation2{2,3,4,5,6,0};
    vector<char> expectedQuery2{'q','r','s','t','u','a'};
    Status expectedStatus2 = Completed;

    EXPECT_CALL(*mockSelector,selectCharacter())
        .Times(11)
        .WillOnce(Return('p'))
        .WillOnce(Return('q'))
        .WillOnce(Return('t'))
        .WillOnce(Return('p'))
        .WillOnce(Return('q'))
        .WillOnce(Return('p'))

        .WillOnce(Return('q'))
        .WillOnce(Return('r'))
        .WillOnce(Return('s'))
        .WillOnce(Return('t'))
        .WillOnce(Return('u'));
    EXPECT_CALL(*mockSelector,getSeed())
        .Times(1)
        .WillOnce(Return(0));

    MonkeyTyper test(0,std::move(mockSelector),query);
    
    moveStreamTestHelper(test, size, expectedStream1, expectedCorrectness1, expectedLocation1, expectedQuery1, expectedStatus1,query,stream1,0);
    moveStreamTestHelper(test, size, expectedStream2, expectedCorrectness2, expectedLocation2, expectedQuery2, expectedStatus2,query,stream1+stream2,0);
}

TEST(MonkeyTyperStreamTest,KillStream){
    unique_ptr<MockLetterSelector> mockSelector = make_unique<MockLetterSelector>();
    EXPECT_CALL(*mockSelector,selectCharacter()).Times(0);
    EXPECT_CALL(*mockSelector,getSeed())
        .Times(1)
        .WillOnce(Return(0));
    queue<int> x;
    x.push(4);
    
    //PositionHolder intentionally set to crash if it gets called to evaluateSelection.
    PositionHolder currSpot("",x,4);
    MonkeyTyper test(0, std::move(mockSelector), currSpot, "", 10);
    test.killStream();

    EXPECT_EQ(Killed,test.moveStream(10));
}

TEST(MonkeyTyperStreamTest,PauseStream){
    unique_ptr<MockLetterSelector> mockSelector = make_unique<MockLetterSelector>();
    std::string query = "xyz";
    int size = 7;
    std::string stream1 = "xzxyxyz";
    vector<char> expectedStream(stream1.begin(),stream1.end());
    vector<LetterOutcome> expectedCorrectness{Match,NoMatch,Match,Match,Fallback,Match,Complete};
    vector<int> expectedLocation{1,0,1,2,1,2,3};
    vector<char> expectedQuery{'x','y','x','y','x','y','z'};
    Status expectedStatus = Completed;

    EXPECT_CALL(*mockSelector,selectCharacter())
        .Times(7)
        .WillOnce(Return('x'))
        .WillOnce(Return('z'))
        .WillOnce(Return('x'))
        .WillOnce(Return('y'))
        .WillOnce(Return('x'))
        .WillOnce(Return('y'))
        .WillOnce(Return('z'));
    EXPECT_CALL(*mockSelector,getSeed())
        .Times(1)
        .WillOnce(Return(0));
    MonkeyTyper test(0,std::move(mockSelector),query);
    
    test.pause();
    //If moveStream moves uses selectCharacter even once, the next moveStream call will fail.
    EXPECT_EQ(test.moveStream(7),Paused);
    EXPECT_EQ(test.getPromptRecord(),0);

    test.unpause();
    moveStreamTestHelper(test, size, expectedStream, expectedCorrectness, expectedLocation, expectedQuery, expectedStatus,query,stream1,0);
}

TEST(MonkeyTyperStreamTest,CompleteSupercedingRest){
    unique_ptr<MockLetterSelector> mockSelector = make_unique<MockLetterSelector>();
    EXPECT_CALL(*mockSelector,selectCharacter())
        .Times(1)
        .WillOnce(Return('a'));
    EXPECT_CALL(*mockSelector,getSeed())
        .Times(1)
        .WillOnce(Return(0));
    std::string query = "a";
    int size = 0;
    MonkeyTyper test(0,std::move(mockSelector),query);
    EXPECT_EQ(test.moveStream(1),Completed);
    test.pause();
    EXPECT_EQ(test.moveStream(1),Completed);
    test.killStream();
    EXPECT_EQ(test.moveStream(1),Completed);
}

int main(int argc, char **argv) {
    cout << "HELLO";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
