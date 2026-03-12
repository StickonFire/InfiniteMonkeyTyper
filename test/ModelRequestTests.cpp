#include "Model.hpp"
#include "ModelRequests.hpp"
#include "mockClasses.hpp"

#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::Test;
using ::testing::Mock;

class ModelRequestTestCalls: public testing::Test {
    protected:
        void runTest(std::unique_ptr<ModelRequest> request, AbstractModel *model){
            request->dispatch(model);
        }
};

TEST_F(ModelRequestTestCalls,Pause){
    unsigned int expectedId = 10;
    std::unique_ptr<PauseRequest> request = make_unique<PauseRequest>(expectedId);
    MockModel model;
    EXPECT_CALL(model,doPauseMonkeyTyper(expectedId))
        .Times(1);
    runTest(std::move(request),&model);
}

TEST_F(ModelRequestTestCalls,Unpause){
    unsigned int expectedId = 95;
    std::unique_ptr<UnpauseRequest> request = make_unique<UnpauseRequest>(expectedId);
    MockModel model;
    EXPECT_CALL(model,doUnpauseMonkeyTyper(expectedId))
        .Times(1);
    runTest(std::move(request),&model);
}

TEST_F(ModelRequestTestCalls,Remove){
    unsigned int expectedId = 20000;
    std::unique_ptr<RemoveRequest> request = make_unique<RemoveRequest>(expectedId);
    MockModel model;
    EXPECT_CALL(model,doRemoveMonkeyTyper(expectedId))
        .Times(1);
    runTest(std::move(request),&model);
}

TEST_F(ModelRequestTestCalls,Create){
    std::string expectedQuery = "abcd";
    unsigned int expectedSeed = 25;
    std::unique_ptr<CreateRequest> request = make_unique<CreateRequest>(expectedQuery,expectedSeed);
    MockModel model;
    EXPECT_CALL(model,doCreateMonkeyTyper(expectedQuery,expectedSeed))
        .Times(1);
    runTest(std::move(request),&model);
}