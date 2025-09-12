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

void positionHolderTestHelper(PositionHolder &test, char selection, int expectedPromptRecord, int expectedHighestSpot, queue<int> &expectedCurrSpot,bool expectedComplete, string testName=""){
    test.evaluateSelection(selection);

    EXPECT_EQ(test.getHighestSpot(),expectedHighestSpot) << "Issue with the highest spot determined. Test " << testName;
    EXPECT_EQ(test.promptRecord, expectedPromptRecord) << "Issue with the promptRecord Test " << testName;
    if(!expectedComplete)
        EXPECT_TRUE(checkQueueEquality(test.currentSpot,expectedCurrSpot)) << "There is an incorrect spot indicated in currSpot. Test " << testName;
    EXPECT_EQ(test.complete,expectedComplete) << "The complete flag is incorrectly set. Test " << testName;
}

TEST(PositionHolderEvalSelectionTest,EmptyCurrSpotCheck){
    PositionHolder test("b",queue<int>(),0);
    positionHolderTestHelper(test,'a',0,0,queue<int>(),false);

    queue<int> expectedResult;
    expectedResult.push(1);
    positionHolderTestHelper(test,'b',1,1,expectedResult,true);
}

TEST(PositionHolderEvalSelectionTest,SingleEntryInCurrSpot){
    queue<int> singular;
    singular.push(1);
    PositionHolder bothFail("ab",queue<int>(singular),1);
    positionHolderTestHelper(bothFail,'c',1,0,queue<int>(),false,"Both characters incorrect.");

    PositionHolder firstCharCorrect("ab",queue<int>(singular),1);
    positionHolderTestHelper(firstCharCorrect,'a',1,1,queue<int>(singular),false,"Only first query char correct.");

    queue<int> expect2;
    expect2.push(2);
    PositionHolder secondCharCorrect("ab",queue<int>(singular),1);
    positionHolderTestHelper(secondCharCorrect,'b',2,2,queue<int>(expect2),true,"Only second query char correct.");

    queue<int> expectBoth(expect2);
    expectBoth.push(1);
    PositionHolder bothCorrect("aa",queue<int>(singular),1);
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
}

int main(int argc, char **argv) {
    cout << "HELLO";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
