#include "mockClasses.hpp"
#include "MonkeyTyper.hpp"
#include <gtest/gtest.h>


struct testInput{
  string query;
  vector<char> rngDraws;
  vector<Status> results;
  vector<int> drawSize;
};

void testValues(testInput &input){
  MockLetterSelector test_rng(input.rngDraws);
  MonkeyTyper test_typer(0,test_rng,input.query);
  Status result;
  for(int i = 0; i < input.drawSize.size(); i++){
      result = test_typer.moveStream(input.drawSize[i]);
      EXPECT_EQ(result,input.results[i]);
  }
}

TEST(MonkeyTyperMoveStreamTest,SingleCharIncorrect) {
  
}

TEST(MonkeyTyperMoveStreamTest,SingleCharCorrectAtStart) {

}

TEST(MonkeyTyperMoveStreamTest,SingleCharCorrectAtEnd) {

}

TEST(MonkeyTyperMoveStreamTest,StreamMatchesQuery) {

}

TEST(MonkeyTyperMoveStreamTest,StreamStartsCorrectThenIncorrect) {

}

TEST(MonkeyTyperMoveStreamTest,StreamCorrectAfterIncorrect){

}

TEST(MonkeyTyperMoveStreamTest,CounterCheckOneStream){

}

TEST(MonkeyTyperMoveStreamTest,DoubleStreamCorrect){

}

TEST(MonkeyTyperMoveStreamTest,DoubleStreamInCorrect){

}

TEST(MonkeyTyperMoveStreamTest,DoubleStreamCounters){

}

TEST(MonkeyTyperMoveStreamTest,CallMoveAfterCompletion){

}

TEST(MonkeyTyperMoveStreamTest,UseNonMockRNG){

}

int main(int argc, char **argv) {
  cout << "HELLO";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}