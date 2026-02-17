#include "RingLeader.hpp"
#include "MonkeyTyper.hpp"
#include "mockClasses.hpp"
#include "Model.hpp"

#include <memory>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::Test;
using ::testing::Mock;
using ::testing::_;


TEST(ModelTest,ConstructorEmptyRingLeader){
    std::map<int,MonkeyTyper> typers;
    unique_ptr<IdMaker> idGenerator;
    int runSize = 1;
    std::map<int,std::string> expectedMessages;
    std::map<int,TyperInfo> expectedInfo;
    std::unique_ptr<RingLeader> RingLeaderPtr = make_unique<RingLeader>(typers,std::move(idGenerator));
    Model test(std::move(RingLeaderPtr),runSize);
    ModelInfo expected(expectedMessages,expectedInfo);
    EXPECT_EQ(test.modelInfo(),expected);
    EXPECT_EQ(test.getRunSize(),runSize);
}

