#include "mockClasses.hpp"
#include "MonkeyTyper.hpp"
#include <gtest/gtest.h>
#include <vector>


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
    vector<int> drawSize({3});

    testValues(query,rngDraws,vector<Status>({results}),drawSize);
}

TEST(MonkeyTyperMoveStreamTest,SingleCharCorrectAtEnd) {
    string query = "a";
    vector<char> rngDraws({'b','a'});
    Status results = {
        vector<TypedChar>({
            TypedChar{'b',1},
            TypedChar{'a',1}
        }),
        true
    };
    vector<int> drawSize({2});

    testValues(query,rngDraws,vector<Status>({results}),drawSize);
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

    testValues(query,rngDraws,vector<Status>({results}),drawSize);
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
    vector<int> drawSize({3});

    testValues(query,rngDraws,vector<Status>({results}),drawSize);
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

    testValues(query,rngDraws,vector<Status>({results}),drawSize);
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
    testValues(query1,rngDraws1,vector<Status>({results1}),drawSize1);

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
    testValues(query2,rngDraws2,vector<Status>({results2}),drawSize2);
}

TEST(MonkeyTyperMoveStreamTest,DoubleStreamCorrect){
    string query1 = "abab";
    vector<char> rngDraws1({'a','d','a','b','a','b','a','d'});
    Status results1 = {
        vector<TypedChar>({
            TypedChar{'a',1},
            TypedChar{'d',2},
            TypedChar{'a',3},
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
    testValues(query1,rngDraws1,vector<Status>({results1,results2}),drawSize1);
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

TEST(MonkeyTyperMoveStreamTest,UseNonMockRNG){
        
}

int main(int argc, char **argv) {
    cout << "HELLO";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}