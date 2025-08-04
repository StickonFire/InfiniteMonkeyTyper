#include "mockClasses.hpp"
#include "MonkeyTyper.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <thread>

/**
 * This configuration tests moveStream itself.
 */

struct testInput {
    string query;
    vector<char> rngDraws;
    vector<Status> results;
    vector<int> drawSize;
};

void testValues(string &query, vector<char> &rngDraws, vector<Status>& results, vector<int>& drawSize){
    MockLetterSelector test_rng(rngDraws);
    MonkeyTyper test_typer(0,&test_rng,query);
    Status given_result;
    for(int i = 0; i < drawSize.size(); i++){
            given_result = test_typer.moveStream(drawSize[i]);
            EXPECT_TRUE(given_result == results[i]) << "Status of test mismatches expected result! EXPECTED: " << std::endl << results[i].toString() << std::endl
                << " RECEIVED: " << std::endl << given_result.toString();
    }
}

TEST(MonkeyTyperMoveStreamTest,SingleCharIncorrect) {
    string query = "a";
    vector<char> rngDraws({'b','b','b'});
    vector<Status> results({
        Status{
            vector<TypedChar>(
                {TypedChar{'b',0},
                 TypedChar{'b',0},
                 TypedChar{'b',0}})
            ,false}
    });
    vector<int> drawSize({3});
    testValues(query,rngDraws,results,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,SingleCharCorrectAtStart) {
    string query = "a";
    vector<char> rngDraws({'a','b','b'});
    Status results = {
        vector<TypedChar>({
            TypedChar{'a',1}
        }),
        true
    };
    vector<Status> result_list({results});
    vector<int> drawSize({3});
    testValues(query,rngDraws,result_list,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,SingleCharCorrectAtEnd) {
    string query = "a";
    vector<char> rngDraws({'b','a'});
    Status results = {
        vector<TypedChar>({
            TypedChar{'b',0},
            TypedChar{'a',1}
        }),
        true
    };
    vector<int> drawSize({2});
    vector<Status> result_list({results});
    testValues(query,rngDraws,result_list,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,StreamMatchesQuery) {
    string query = "aaa";
    vector<char> rngDraws({'a','a','a'});
    Status results = {
        vector<TypedChar>({
            TypedChar{'a',1},
            TypedChar{'a',2},
            TypedChar{'a',3}
        }),
        true
    };
    vector<int> drawSize({3});
    vector<Status> result_list({results});
    testValues(query,rngDraws,result_list,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,StreamStartsCorrectThenIncorrect) {
    string query = "ab";
    vector<char> rngDraws({'a','a','a'});
    Status results = {
        vector<TypedChar>({
            TypedChar{'a',1},
            TypedChar{'a',1},
            TypedChar{'a',1}
        }),
        false
    };
    vector<Status> result_list = vector<Status>({results});
    vector<int> drawSize({3});

    testValues(query,rngDraws,result_list,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,StreamCorrectAfterIncorrect){
    string query = "aa";
    vector<char> rngDraws({'b','a','a'});
    Status results = {
        vector<TypedChar>({
            TypedChar{'b',0},
            TypedChar{'a',1},
            TypedChar{'a',2}
        }),
        true
    };
    vector<int> drawSize({3});
    vector<Status> result_list({results});
    testValues(query,rngDraws,result_list,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,CounterCheckOneStream){
    string query1 = "abab";
    vector<char> rngDraws1({'a','b','a','b','a','b'});
    Status results1 = {
        vector<TypedChar>({
            TypedChar{'a',1},
            TypedChar{'b',2},
            TypedChar{'a',3},
            TypedChar{'b',4},
        }),
        true
    };
    vector<int> drawSize1 = {6};
    vector<Status> result_list1({results1});
    testValues(query1,rngDraws1,result_list1,drawSize1);

    string query2 = "abad";
    vector<char> rngDraws2({'a','b','a','b','a','d'});
    Status results2 = {
        vector<TypedChar>({
            TypedChar{'a',1},
            TypedChar{'b',2},
            TypedChar{'a',3},
            TypedChar{'b',2},
            TypedChar{'a',3},
            TypedChar{'d',4}
        }),
        true
    };
    vector<int> drawSize2 = {6};
    vector<Status> result_list2({results2});
    testValues(query2,rngDraws2,result_list2,drawSize2);
}

TEST(MonkeyTyperMoveStreamTest,DoubleStreamCorrect){
    string query1 = "abab";
    vector<char> rngDraws1({'a','d','a','b','a','b','a','d'});
    Status results1 = {
        vector<TypedChar>({
            TypedChar{'a',1},
            TypedChar{'d',0},
            TypedChar{'a',1},
            TypedChar{'b',2}
        }),
        false
    };
    Status results2 = {
        vector<TypedChar>({
            TypedChar{'a',3},
            TypedChar{'b',4}
        }),
        true
    };
    vector<int> drawSize1 = {4,4};
    vector<Status> expected_results({results1,results2});
    testValues(query1,rngDraws1,expected_results,drawSize1);
}

TEST(MonkeyTyperMoveStreamTest,DoubleStreamInCorrect){
    string query = "aa";
    vector<char> rngDraws({'a','b','a','b','a','b'});
    vector<Status> results({
        Status{
            vector<TypedChar>({
                TypedChar{'a',1},
                TypedChar{'b',0},
                TypedChar{'a',1}
            }),
            false
        },
        Status{
            vector<TypedChar>({
                TypedChar{'b',0},
                TypedChar{'a',1},
                TypedChar{'b',0}
            }),
            false
        }
    });
    vector<int> drawSize({3,3});
    testValues(query,rngDraws,results,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,DoubleStreamCounters){
    string query = "abad";
    vector<char> rngDraws({'a','b','a','b','a','d','a','b'});
    vector<Status> results({
        Status{
            vector<TypedChar>({
                TypedChar{'a',1},
                TypedChar{'b',2},
                TypedChar{'a',3},
                TypedChar{'b',2}
            }),
            false
        },
        Status{
            vector<TypedChar>({
                TypedChar{'a',3},
                TypedChar{'d',4},
            }),
            true
        }
    });
    vector<int> drawSize({4,4});
    testValues(query,rngDraws,results,drawSize);
}

TEST(MonkeyTyperMoveStreamTest,CallMoveAfterCompletion){
    string query = "a";
    vector<char> rngDraws({'a','b'});
    vector<Status> results({
        Status{
            vector<TypedChar>({
                TypedChar{'a',1}
            }),
            true
        },
        Status{
            vector<TypedChar>(),
            true
        }
    });
    vector<int> drawSize({1,1});
    testValues(query,rngDraws,results,drawSize);
}

/**
 * This configuration tests the stream function
 */

TEST(MonkeyTyperStreamTest,StreamNodeTest){
    vector<char> returnValues{'b','a'};
    MockLetterSelector rng(returnValues);

    string query = "a";
    int id = 10;
    MonkeyTyper test(id,&rng,query,1);
    queue<TyperMessage> results;
    
    queue<TyperMessage> expectedValues;
    expectedValues.push(TyperMessage{id,Status{vector<TypedChar>{TypedChar{'b',0}},false}});
    expectedValues.push(TyperMessage{id,Status{vector<TypedChar>{TypedChar{'a',1}},true}});

    for(int i = 0; i < returnValues.size(); i++)
        test.stream(results);
    
    EXPECT_EQ(expectedValues,results) << "Stream function failed to provide expected results.";
}

TEST(MonkeyTyperStreamTest,StartTwoStreamProcesses){
    mt19937LetterSelector rng("b",0);
    string query = "aaaaa";
    MonkeyTyper test(1,&rng,query);
    queue<TyperMessage> queue1;
    queue<TyperMessage> queue2;

    thread first = test.startStream(queue1);
    thread second = test.startStream(queue2);
    cout << "Survived creating threads.\n";

    this_thread::sleep_for(10ms);
    test.killStream();
    this_thread::sleep_for(10ms);
    cout << "Queue Sizes, 1: " << queue1.size() << " 2: " << queue2.size() << "\n";
    bool onlyOneRan = (queue1.size() > 0 && queue2.size() == 0) || (queue1.size() == 0 && queue2.size() > 0);
    cout << onlyOneRan << "\n";
    EXPECT_TRUE(onlyOneRan) << "Error with the queue results.";
    cout << "Survived Expectation";
    first.join();
    second.join();
}

TEST(MonkeyTyperStreamTest,StreamToCompletion){
    vector<char> returnValues{'b','a'};
    MockLetterSelector rng(returnValues);
    
    string query = "a";
    int id = 10;
    MonkeyTyper test(id,&rng,query,1);
    queue<TyperMessage> results;
    
    queue<TyperMessage> expectedValues;
    expectedValues.push(TyperMessage{id,Status{vector<TypedChar>{TypedChar{'b',0}},false}});
    expectedValues.push(TyperMessage{id,Status{vector<TypedChar>{TypedChar{'a',1}},true}});

    thread first = test.startStream(results);
    
    first.join();

    
    EXPECT_EQ(results,expectedValues) << "MonkeyTyper stream did not provide the expected result.";

    queue<TyperMessage> shouldEqual(results);

    thread next = test.startStream(shouldEqual);
    next.join();

    EXPECT_EQ(results,shouldEqual) << "MonkeyTyper stream let the system restart streaming when it is already completed.";
}

TEST(MonkeyTyperStreamTest,CheckPausingPriorToStartStream){
    vector<char> returnValues;
    MockLetterSelector rng(returnValues);

    string query = "a";
    MonkeyTyper test(21,&rng,query);
    queue<TyperMessage> results;

    test.pause();

    thread next = test.startStream(results);
    this_thread::sleep_for(50ms);
    EXPECT_EQ(results.size(),0) << "MonkeyTyper Stream is paused, shouldn't have received anything.";
    test.killStream();
    next.join();
}

TEST(MonkeyTyperStreamTest,CheckPauseInteractions){
    mt19937LetterSelector rng("b",0);

    string query = "aaaaa";
    MonkeyTyper test(31,&rng,query);
    queue<TyperMessage> results;

    thread next = test.startStream(results);
    this_thread::sleep_for(50ms);
    test.pause();
    this_thread::sleep_for(50ms);
    queue<TyperMessage> same(results);
    this_thread::sleep_for(50ms);

    ASSERT_EQ(same,results) << "MonkeyTyper stream is paused, queue received more after it was paused midway.";

    test.pause();
    this_thread::sleep_for(50ms);

    ASSERT_EQ(results,same) << "MonkeyTyper pause shouldn't have unpaused if it is called twice.";

    test.unpause();

    this_thread::sleep_for(50ms);
    test.pause();
    this_thread::sleep_for(50ms);
    EXPECT_NE(results,same) << "MonkeyTyper unpause should allow the MonkeyTyper to add to the queue.";

    test.killStream();
    next.join();
}

TEST(MonkeyTyperStreamTest,CheckKillStream){
    mt19937LetterSelector rng("b",0);

    string query = "aaaaa";
    MonkeyTyper test(41,&rng,query);

    queue<TyperMessage> results;
    thread testStream = test.startStream(results);
    this_thread::sleep_for(50ms);
    test.pause();
    this_thread::sleep_for(50ms);

    queue<TyperMessage> expectedToEqual(results);

    test.killStream();
    this_thread::sleep_for(10ms);
    test.unpause();

    this_thread::sleep_for(50ms);
    EXPECT_EQ(expectedToEqual,results) << "Unpausing when the stream is killed caused the stream to continue when it shouldn't.";

    thread newTestStream = test.startStream(results);
    this_thread::sleep_for(50ms);
    test.pause();
    EXPECT_NE(expectedToEqual,results) << "Starting a new stream failed to continue operation.";

    test.killStream();
    testStream.join();
    newTestStream.join();
}

int main(int argc, char **argv) {
    cout << "HELLO";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
