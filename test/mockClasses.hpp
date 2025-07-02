#include <random>
using namespace std;

class MockRandom : mt19937 {
    int operator()();
};