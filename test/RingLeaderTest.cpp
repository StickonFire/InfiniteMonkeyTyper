#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"

#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::Test;
using ::testing::Mock;
using ::testing::_;

template <class T>
ostream& operator<<(ostream& os, vector<T> other){
    os << "vector: {";
    for(T element : other){
        os << element << " ";
    }
    os << "}";
    return os;
}

namespace {
    template<typename Contained>
    struct VectorSlicer{
        std::vector<Contained> container;
        typename std::vector<Contained>::iterator containerItr;

        VectorSlicer(std::vector<Contained> container): container(container), containerItr(this->container.begin()) { }

        std::vector<Contained> slice(int size){
            typename std::vector<Contained>::iterator end = containerItr;
            end += size;
            std::vector<Contained> result(containerItr,end);
            containerItr = end;
            return result;
        }
    };

    class ExpectedListInfoConstructor{
        int id;
        VectorSlicer<char> expectedStream;
        VectorSlicer<char> expectedCorrespondingQuery;
        VectorSlicer<LetterOutcome> expectedOutcome;
        VectorSlicer<int> expectedBestLocation;

        VectorSlicer<int> expectedCurrentLocation;
        VectorSlicer<int> expectedRecord;
        VectorSlicer<int> expectedSize;

        public:
            ExpectedListInfoConstructor(int id,std::vector<int> expectedCurrentLocation, std::vector<int> expectedSize, std::vector<int> expectedRecord, std::vector<char> expectedStream, std::vector<LetterOutcome> expectedOutcome, std::vector<char> expectedCorrespondingQuery, 
                std::vector<int> expectedBestLocation) :
                id(id), expectedStream(expectedStream), expectedCorrespondingQuery(expectedCorrespondingQuery), expectedOutcome(expectedOutcome), expectedBestLocation(expectedBestLocation),
                expectedCurrentLocation(expectedCurrentLocation), expectedRecord(expectedRecord), expectedSize(expectedSize) { };

            ListInfo generateNextListInfo(int size){
                vector<char> stream = expectedStream.slice(size);
                vector<LetterOutcome> outcome = expectedOutcome.slice(size);
                vector<char> corresponding = expectedCorrespondingQuery.slice(size);
                vector<int> bestLocation = expectedBestLocation.slice(size);
                int currentLocation = expectedCurrentLocation.slice(1)[0];
                int streamSize = expectedSize.slice(1)[0];
                int record = expectedRecord.slice(1)[0];
                return ListInfo(id,currentLocation,streamSize,record,stream,outcome,corresponding,bestLocation);
            }

            ListInfo generateEmptyListInfo(){
                vector<char> emptyChar;
                vector<LetterOutcome> emptyOutcome;
                vector<int> emptyInt;
                return ListInfo(id,0,0,0,emptyChar,emptyOutcome,emptyChar,emptyInt);
            }
    };

    class ExpectedTyperInfoConstructor{
        VectorSlicer<std::string> streams;
        std::string prompt;
        unsigned int seed;
        
        public:
            ExpectedTyperInfoConstructor(std::vector<std::string> streams,std::string prompt,unsigned int seed):
                streams(streams),prompt(prompt),seed(seed) { }

            TyperInfo generateNextTyperInfo(ListInfo listInfo){
                return TyperInfo(listInfo,streams.slice(1)[0],prompt,seed);
            }

            TyperInfo generateEmptyTyperInfo(int id){
                ListInfo empty;
                empty.id = id;
                return TyperInfo(empty,std::string(""),prompt,seed);
            }
    };
}

class RingLeaderWholisticTestSuite : public testing::Test {
    protected:
        std::vector<ListInfo> expectedListInfo;
        std::map<int,TyperInfo> expectedTyperInfo;
        std::vector<MonkeyTyperStatus> expectedStatus;

        void checkInfoStructs(RingLeader &test, std::string message){
            EXPECT_EQ(test.listInfo(),expectedListInfo) << message << "ListInfo failed to match!";
            EXPECT_EQ(test.typerInfo(),expectedTyperInfo) << message << "TyperInfos failed to match!";
        }
};

/**
 * Wholistic tests for the following functions:
 *  - Constructor
 *  - runNCharacters
 *  - listInfo
 *  - promptInfo
 *  - streamInfo
 */
TEST(TestApparatusCheck,SliceTwice){
    vector<int> toSlice{0,1,2};
    VectorSlicer<int> test(toSlice);

    EXPECT_EQ(test.slice(1),vector<int>{0});
    EXPECT_EQ(test.slice(2),(vector<int>{1,2}));
}

TEST(TestApparatusCheck,ExpectedListInfoConstructorTest){
    int id = 200;
    vector<char> stream{'a','b','c'};
    vector<LetterOutcome> outcome{Match,NoMatch,Fallback};
    vector<char> corresponding{'d','e','f'};
    vector<int> location{1,2,3};
    vector<int> currentLocation{1,3};
    vector<int> expectedRecord{5,6};
    vector<int> expectedSize{1,3};
    ExpectedListInfoConstructor test(id,currentLocation,expectedSize,expectedRecord,stream,outcome,corresponding,location);
    
    stream = vector<char>{'a'};
    outcome = vector<LetterOutcome>{Match};
    corresponding = vector<char>{'d'};
    location = vector<int>{1};
    ListInfo firstExpectation(id,location[0],expectedSize[0],expectedRecord[0],stream,outcome,corresponding,location);
    EXPECT_EQ(firstExpectation,test.generateNextListInfo(1));

    stream = vector<char>{'b','c'};
    outcome = vector<LetterOutcome>{NoMatch,Fallback};
    corresponding = vector<char>{'e','f'};
    location = vector<int>{2,3};
    ListInfo secondExpectation(id,location[1],expectedSize[1],expectedRecord[1],stream,outcome,corresponding,location);
    EXPECT_EQ(secondExpectation,test.generateNextListInfo(2));
}

TEST_F(RingLeaderWholisticTestSuite,EmptyList){
    std::map<int,MonkeyTyper> empty;
    std::set<int> usedIds;
    unique_ptr<CounterIdMaker> idGenerator = make_unique<CounterIdMaker>(50,usedIds);
    RingLeader test(empty,std::move(idGenerator));

    checkInfoStructs(test,"Case no monkeyTypers, and no running.");

    EXPECT_EQ(test.runNCharacters(100),expectedStatus);

    checkInfoStructs(test,"Case no monkeyTypers, and has run typers once.");
}

TEST_F(RingLeaderWholisticTestSuite,OneMonkeyTyper){
    std::string query = "ab";
    unsigned int seed = 0;
    int monkeyTyperId = 1;
    vector<int> sizeOfRuns{1,1};
    ExpectedListInfoConstructor listInfoConstructor(
        monkeyTyperId,
        std::vector<int>{1,2},
        std::vector<int>{1,2},
        std::vector<int>{1,2},
        std::vector<char>{'a','b'},
        std::vector<LetterOutcome>{Match,Complete},
        std::vector<char>{'a','b'},
        std::vector<int>{1,2}
    );
    ExpectedTyperInfoConstructor typerInfoConstructor(
        std::vector<std::string>{"a","ab"},
        query,
        seed
    );

    std::map<int,MonkeyTyper> onlyOne;
    unique_ptr<MockLetterSelector> selector = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector,selectCharacter())
        .Times(2)
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));
    MonkeyTyper onlyTyper(monkeyTyperId,std::move(selector),"ab");
    onlyTyper.moveStream(1);
    onlyOne.insert(std::make_pair(1,std::move(onlyTyper)));

    std::set<int> usedIds;
    unique_ptr<CounterIdMaker> idGenerator = make_unique<CounterIdMaker>(50,usedIds);
    RingLeader test(onlyOne,std::move(idGenerator));
    
    expectedListInfo.push_back(listInfoConstructor.generateNextListInfo(1));
    expectedTyperInfo[1] = typerInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    checkInfoStructs(test,"Case single typer.");

    vector<MonkeyTyperStatus> expectedStatus{MonkeyTyperStatus(1,Completed)};
    EXPECT_EQ(test.runNCharacters(1),expectedStatus);

    expectedListInfo[0] = listInfoConstructor.generateNextListInfo(1);
    expectedTyperInfo[1] = typerInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);

    checkInfoStructs(test,"Case single typer.");
}

TEST_F(RingLeaderWholisticTestSuite,TwoMonkeysOneUnrun){
    int firstId = 1;
    int secondId = 2;
    unsigned int firstSeed = 2;
    unsigned int secondSeed = 2;
    std::string firstQuery = "aa";
    std::string secondQuery = "c";

    ExpectedListInfoConstructor firstListInfoConstructor(
        firstId,
        std::vector<int>{1,0},
        std::vector<int>{1,2},
        std::vector<int>{1,1},
        std::vector<char>{'a','b'},
        std::vector<LetterOutcome>{Match,NoMatch},
        std::vector<char>{'a','a'},
        std::vector<int>{1,0}
    );
    ExpectedTyperInfoConstructor firstTyperInfoConstructor(
        std::vector<std::string>{"a","ab"},
        firstQuery,
        firstSeed
    );
    ExpectedListInfoConstructor secondListInfoConstructor(
        secondId,
        std::vector<int>{1},
        std::vector<int>{1},
        std::vector<int>{1},
        std::vector<char>{'c'},
        std::vector<LetterOutcome>{Complete},
        std::vector<char>{'c'},
        std::vector<int>{1}
    );
    ExpectedTyperInfoConstructor secondTyperInfoConstructor(
        std::vector<std::string>{"c"},
        secondQuery,
        secondSeed
    );


    std::map<int,MonkeyTyper> two;
    unique_ptr<MockLetterSelector> firstSelector = make_unique<MockLetterSelector>();
    unique_ptr<MockLetterSelector> secondSelector = make_unique<MockLetterSelector>();

    EXPECT_CALL(*firstSelector,selectCharacter())
        .Times(2)
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));
    EXPECT_CALL(*firstSelector,getSeed())
        .Times(1)
        .WillOnce(Return(firstSeed));
    EXPECT_CALL(*secondSelector,selectCharacter())
        .Times(1)
        .WillOnce(Return('c'));
    EXPECT_CALL(*secondSelector,getSeed())
        .Times(1)
        .WillOnce(Return(secondSeed));

    MonkeyTyper firstTyper(firstId,std::move(firstSelector),firstQuery);
    firstTyper.moveStream(1);
    MonkeyTyper secondTyper(secondId,std::move(secondSelector),"c");
    two.insert(std::make_pair(firstId,std::move(firstTyper)));
    two.insert(std::make_pair(secondId,std::move(secondTyper)));

    unique_ptr<CounterIdMaker> idMaker = make_unique<CounterIdMaker>(100,std::set<int>());
    RingLeader test(two,std::move(idMaker));

    expectedListInfo.push_back(firstListInfoConstructor.generateNextListInfo(1));
    expectedListInfo.push_back(secondListInfoConstructor.generateEmptyListInfo());

    expectedTyperInfo[firstId] = firstTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    expectedTyperInfo[secondId] = secondTyperInfoConstructor.generateEmptyTyperInfo(secondId);

    checkInfoStructs(test,"Case Two MonkeyTypers, with one unrun.");

    std::vector<MonkeyTyperStatus> expectedStatus{MonkeyTyperStatus(1,PacketReady),MonkeyTyperStatus(2,Completed)};
    EXPECT_EQ(test.runNCharacters(1),expectedStatus);

    expectedListInfo[0] = firstListInfoConstructor.generateNextListInfo(1);
    expectedListInfo[1] = secondListInfoConstructor.generateNextListInfo(1);
    expectedTyperInfo[firstId] = firstTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    expectedTyperInfo[secondId] = secondTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[1]);

    checkInfoStructs(test,"Case Two MonkeyTypers, with both run");
}

/**
 * This tests all 4 combinations 2 monkeyTypers can have with their pause status.
 * These combinations are the following:
 * Runs:         1       2        3      4
 * MonkeyTyper1: Pause   Unpaused Paused Unpaused
 * MonkeyTyper2: Unpause Paused   Paused Unpaused
 */
TEST_F(RingLeaderWholisticTestSuite,PauseTests){    
    std::string query1 = "abc";
    int id1 = 0;
    std::string query2 = "cd";
    int id2 = 1;
    vector<int> runSize{1,2};
    ExpectedListInfoConstructor firstListInfoConstructor(
        id1,
        std::vector<int>{1,3},
        std::vector<int>{1,3},
        std::vector<int>{1,3},
        std::vector<char>{'a','b','c'},
        std::vector<LetterOutcome>{Match,Match,Complete},
        std::vector<char>{'a','b','c'},
        std::vector<int>{1,2,3}
    );
    ExpectedTyperInfoConstructor firstTyperInfoConstructor(
        std::vector<std::string>{"a","abc"},
        query1,
        id1
    );
    ExpectedListInfoConstructor secondListInfoConstructor(
        id2,
        std::vector<int>{1,0},
        std::vector<int>{1,3},
        std::vector<int>{1,1},
        std::vector<char>{'c','b','d'},
        std::vector<LetterOutcome>{Match,NoMatch,NoMatch},
        std::vector<char>{'c','d','c'},
        std::vector<int>{1,0,0}
    );
    ExpectedTyperInfoConstructor secondTyperInfoConstructor(
        std::vector<std::string>{"c","cbd"},
        query2,
        id2
    );

    std::map<int,MonkeyTyper> two;
    unique_ptr<MockLetterSelector> selector1 = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector1,getSeed())
        .Times(1)
        .WillOnce(Return(id1));
    EXPECT_CALL(*selector1,selectCharacter())
        .Times(3)
        .WillOnce(Return(query1[0]))
        .WillOnce(Return(query1[1]))
        .WillOnce(Return(query1[2]));
    
    unique_ptr<MockLetterSelector> selector2 = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector2,getSeed())
        .Times(1)
        .WillOnce(Return(id2));
    EXPECT_CALL(*selector2,selectCharacter())
        .Times(3)
        .WillOnce(Return('c'))
        .WillOnce(Return('b'))
        .WillOnce(Return('d'));
    
    MonkeyTyper first(id1,std::move(selector1),query1);
    MonkeyTyper second(id2,std::move(selector2),query2);
    two.insert(std::make_pair(id1,std::move(first)));
    two.insert(std::make_pair(id2,std::move(second)));
    RingLeader test(two,nullptr);

    expectedListInfo.push_back(firstListInfoConstructor.generateEmptyListInfo());
    expectedListInfo.push_back(secondListInfoConstructor.generateEmptyListInfo());

    expectedTyperInfo[id1] = firstTyperInfoConstructor.generateEmptyTyperInfo(id1);
    expectedTyperInfo[id2] = secondTyperInfoConstructor.generateEmptyTyperInfo(id2);
    checkInfoStructs(test,"PauseTest, Case Both Typers Initialized.");


    //First Run. First MT is paused, second unpaused
    test.pauseMonkeyTyper(id1);
    test.unpauseMonkeyTyper(id2);
    EXPECT_EQ(test.runNCharacters(1),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Paused),MonkeyTyperStatus(id2,PacketReady)}));

    expectedListInfo[1] = secondListInfoConstructor.generateNextListInfo(runSize[0]);
    expectedTyperInfo[id2] = secondTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[1]);
    checkInfoStructs(test,"Pause Test. First Paused, Second Unpaused.");

    //second run. first MT unpaused, second paused.
    test.unpauseMonkeyTyper(id1);
    test.pauseMonkeyTyper(id2);
    EXPECT_EQ(test.runNCharacters(1),(vector<MonkeyTyperStatus>{MonkeyTyperStatus{id1,PacketReady},MonkeyTyperStatus{id2,Paused}}));

    expectedListInfo[0] = firstListInfoConstructor.generateNextListInfo(runSize[0]);
    expectedTyperInfo[id1] = firstTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    checkInfoStructs(test,"Pause Test. First unpaused, Second Paused.");

    //Third "run". both MTs paused.
    test.pauseMonkeyTyper(id1);
    test.pauseMonkeyTyper(id2);
    EXPECT_EQ(test.runNCharacters(999),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Paused),MonkeyTyperStatus(id2,Paused)}));
    checkInfoStructs(test,"Pause Test. Both Paused.");
    
    //Fourth run. both MTS unpaused.
    test.unpauseMonkeyTyper(id1);
    test.unpauseMonkeyTyper(id2);
    EXPECT_EQ(test.runNCharacters(2),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Completed),MonkeyTyperStatus(id2,PacketReady)}));

    expectedListInfo[0] = firstListInfoConstructor.generateNextListInfo(runSize[1]);
    expectedTyperInfo[id1] = firstTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    expectedListInfo[1] = secondListInfoConstructor.generateNextListInfo(runSize[1]);
    expectedTyperInfo[id2] = secondTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[1]);
    checkInfoStructs(test,"Pause Test. Both Unpaused.");
}

TEST(RingLeaderTest,CreateAndRemoveMonkeyTypers){
TEST_F(RingLeaderWholisticTestSuite,CreateAndRemoveMonkeyTypers){
    int id1 = 10;
    int id2 = 20;
    unsigned int seed1 = 10;
    unsigned int seed2 = 20;
    std::string query1 = "ab";
    std::string query2 = "abcde";

    ExpectedListInfoConstructor firstListInfoConstructor(
        id1,
        std::vector<int>{1,2},
        std::vector<int>{1,2},
        std::vector<int>{1,2},
        std::vector<char>{'a','b','a'},
        std::vector<LetterOutcome>{Match,Complete,Untracked},
        std::vector<char>{'a','b','a'},
        std::vector<int>{1,2,0}
    );
    ExpectedTyperInfoConstructor firstTyperInfoConstructor(
        std::vector<std::string>{"a","ab"},
        query1,
        seed1
    );
    ExpectedListInfoConstructor secondListInfoConstructor(
        id2,
        std::vector<int>{1,2},
        std::vector<int>{2,3},
        std::vector<int>{1,2},
        std::vector<char>{'a','a','b'},
        std::vector<LetterOutcome>{Match,Fallback,Match},
        std::vector<char>{'a','a','b'},
        std::vector<int>{1,1,2}
    );
    ExpectedTyperInfoConstructor secondTyperInfoConstructor(
        std::vector<std::string>{"aa","aab"},
        query2,
        seed2
    );


    unique_ptr<MockLetterSelector> selector1 = make_unique<MockLetterSelector>();
    unique_ptr<MockLetterSelector> selector2 = make_unique<MockLetterSelector>();
    EXPECT_CALL(*selector1,getSeed())
        .Times(1)
        .WillOnce(Return(seed1));
    EXPECT_CALL(*selector1,selectCharacter())
        .Times(2)
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));

    EXPECT_CALL(*selector2,getSeed())
        .Times(1)
        .WillOnce(Return(seed2));
    EXPECT_CALL(*selector2,selectCharacter())
        .Times(3)
        .WillOnce(Return('a'))
        .WillOnce(Return('a'))
        .WillOnce(Return('b'));
    
    unique_ptr<MockMonkeyTyperFactory> mockFactory = make_unique<MockMonkeyTyperFactory>();
    EXPECT_CALL(*mockFactory,build(id1,seed1,query1))
        .WillOnce(Return(MonkeyTyper(id1,std::move(selector1),query1)));
    EXPECT_CALL(*mockFactory,build(id2,seed2,query2))
        .WillOnce(Return(MonkeyTyper(id2,std::move(selector2),query2)));

    unique_ptr<MockIdMaker> mockId = make_unique<MockIdMaker>();
    EXPECT_CALL(*mockId,generateId())
        .Times(2)
        .WillOnce(Return(id1))
        .WillOnce(Return(id2));
    
    EXPECT_CALL(*mockId,releaseId(_))
        .Times(2);

    std::map<int,MonkeyTyper> empty;
    RingLeader test(empty,std::move(mockId),std::move(mockFactory));

    checkInfoStructs(test,"Initial Check.");

    test.createMonkeyTyper(query1,seed1);

    expectedListInfo.push_back(firstListInfoConstructor.generateEmptyListInfo());
    expectedTyperInfo[id1] = firstTyperInfoConstructor.generateEmptyTyperInfo(id1);
    checkInfoStructs(test,"First creation.");

    int runSize = 1;
    EXPECT_EQ(test.runNCharacters(runSize),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,PacketReady)}));

    expectedListInfo[0] = firstListInfoConstructor.generateNextListInfo(runSize);
    expectedTyperInfo[id1] = firstTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    checkInfoStructs(test,"Run only first.");

    test.createMonkeyTyper(query2,seed2);
    expectedListInfo.push_back(secondListInfoConstructor.generateEmptyListInfo());
    expectedTyperInfo[id2] = secondTyperInfoConstructor.generateEmptyTyperInfo(id2);
    checkInfoStructs(test,"Create Second MonkeyTyper.");

    runSize = 2;
    EXPECT_EQ(test.runNCharacters(runSize),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id1,Completed),MonkeyTyperStatus(id2,PacketReady)}));
    expectedListInfo[0] = firstListInfoConstructor.generateNextListInfo(runSize);
    expectedTyperInfo[id1] = firstTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    expectedListInfo[1] = secondListInfoConstructor.generateNextListInfo(runSize);
    expectedTyperInfo[id2] = secondTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[1]);
    checkInfoStructs(test,"Run Both MonkeyTypers");

    test.removeMonkeyTyper(id1);
    expectedListInfo.erase(expectedListInfo.begin());
    expectedTyperInfo.erase(expectedTyperInfo.find(id1));
    checkInfoStructs(test,"Remove First MonkeyTyper");

    runSize = 1;
    EXPECT_EQ(test.runNCharacters(runSize),(vector<MonkeyTyperStatus>{MonkeyTyperStatus(id2,PacketReady)}));
    expectedListInfo[0] = secondListInfoConstructor.generateNextListInfo(runSize);
    expectedTyperInfo[id2] = secondTyperInfoConstructor.generateNextTyperInfo(expectedListInfo[0]);
    checkInfoStructs(test,"Run Second MonkeyTyper.");

    test.removeMonkeyTyper(id2);
    expectedListInfo.erase(expectedListInfo.begin());
    expectedTyperInfo.erase(expectedTyperInfo.find(id2));
    checkInfoStructs(test,"Remove Second MonkeyTyper");

    EXPECT_EQ(test.runNCharacters(999),vector<MonkeyTyperStatus>());
    checkInfoStructs(test,"Run an empty RingLeader");

    test.removeMonkeyTyper(0);
    checkInfoStructs(test,"Remove Nonexistant Typer");
    EXPECT_EQ(test.runNCharacters(999),vector<MonkeyTyperStatus>());
    checkInfoStructs(test,"Run an empty RingLeader after empty remove called.");
}

/**
 * This test uses the current classes with no mock values. 
 */
TEST(RingLeaderTest,NoMocks){
    unsigned int seed = 200;
    int startCounter = 399;
    int querySize = 94;
    mt19937LetterSelector control(default_alphabet,seed);
    std::string query = "";
    for(int i = 0; i < querySize; i++){
        query.push_back(control.selectCharacter());
    }
    
    unique_ptr<CounterIdMaker> counter = make_unique<CounterIdMaker>(startCounter,std::set<int>());
    unique_ptr<mt19937MonkeyTyperFactory> factory = make_unique<mt19937MonkeyTyperFactory>();
    std::map<int,MonkeyTyper> typers;
    RingLeader test(typers,std::move(counter),std::move(factory));

    test.createMonkeyTyper(query,seed);
    vector<char> expectedStream;
    vector<LetterOutcome> expectedOutcome;
    vector<char> expectedCorresponding;
    vector<int> expectedLocation;
    ListInfo expectedListInfo(startCounter,0,0,0,expectedStream,expectedOutcome,expectedCorresponding,expectedLocation);
    StreamInfo expectedStreamInfo(seed,std::string(""),expectedListInfo);
    PromptInfo expectedPromptInfo(seed,query,expectedListInfo);
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo}));
    EXPECT_EQ(test.promptInfo(startCounter),expectedPromptInfo);
    EXPECT_EQ(test.streamInfo(startCounter),expectedStreamInfo);
    EXPECT_EQ(test.runNCharacters(querySize),(std::vector<MonkeyTyperStatus>{MonkeyTyperStatus(startCounter,Completed)}));
    for(int i = 0; i < querySize; i++){
        expectedStream.push_back(query[i]);
        expectedCorresponding.push_back(query[i]);
        expectedLocation.push_back(i+1);
    }
    expectedOutcome = vector<LetterOutcome>(querySize-1,Match);
    expectedOutcome.push_back(Complete);
    expectedListInfo = ListInfo(startCounter,querySize,querySize,querySize,expectedStream,expectedOutcome,expectedCorresponding,expectedLocation);
    expectedStreamInfo.listInfo = expectedListInfo;
    expectedStreamInfo.stream = query;
    expectedPromptInfo.listInfo = expectedListInfo;
    EXPECT_EQ(test.listInfo(),(vector<ListInfo>{expectedListInfo}));
    EXPECT_EQ(test.promptInfo(startCounter),expectedPromptInfo);
    EXPECT_EQ(test.streamInfo(startCounter),expectedStreamInfo);
}