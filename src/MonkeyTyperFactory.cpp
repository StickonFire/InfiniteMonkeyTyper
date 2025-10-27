#include "MonkeyTyperFactory.hpp"
#include "MonkeyTyper.hpp"

MonkeyTyper mt19937MonkeyTyperFactory::build(int id, unsigned int seed, std::string query){
    unique_ptr<mt19937LetterSelector> selector = make_unique<mt19937LetterSelector>(seed);
    return MonkeyTyper(id,std::move(selector),query);
}