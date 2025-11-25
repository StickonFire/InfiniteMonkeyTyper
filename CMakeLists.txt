cmake_minimum_required(VERSION 3.14)
project(InfiniteMonkeyTyper)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/c67de117379f4d1c889c7581a0a76aa0979c2083.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  MonkeyTyperTest
  src/ModelInfo.cpp
  src/MonkeyTyper.cpp
  src/RingLeader.cpp
  src/MonkeyTyperFactory.cpp
  test/MonkeyTyperTest.cpp
  test/RingLeaderTest.cpp
  test/CounterIdMakerTest.cpp
  test/MonkeyTyperFactoryTest.cpp
  test/InfoStructTest.cpp
)
target_link_libraries(
  MonkeyTyperTest
  GTest::gtest_main
  GTest::gmock_main 
)

target_include_directories( 
  MonkeyTyperTest 
  PUBLIC include
)

include(GoogleTest)
gtest_discover_tests(MonkeyTyperTest)

