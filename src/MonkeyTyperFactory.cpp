#include "MonkeyTyperFactory.hpp"
#include "MonkeyTyper.hpp"

unique_ptr<MonkeyTyper> mt19937MonkeyTyperFactory::build(int id, unsigned int seed, std::string query){
    unique_ptr<mt19937LetterSelector> selector = make_unique<mt19937LetterSelector>(seed);
    return make_unique<MonkeyTyper>(id,std::move(selector),query);
}