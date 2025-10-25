#ifndef MONKEY_TYPER_FACTORY
#define MONKEY_TYPER_FACTORY
#include "MonkeyTyper.hpp"

#include <memory.h>

class MonkeyTyperFactory {
    public:
        /**
         * Returns a constructed MonkeyTyper using the provided values.
         * @param id the id value for the constructed MonkeyTyper
         * @param seed the seed value for the constructed MonkeyTyper
         * @param query the query value for the constructed MonkeyTyper
         * @returns an unrun MonkeyTyper that uses the provided values.
         */
        virtual unique_ptr<MonkeyTyper> build(int id, unsigned int seed, std::string query) = 0;
};

class mt19937MonkeyTyperFactory: public MonkeyTyperFactory {
    public:
        unique_ptr<MonkeyTyper> build(int id, unsigned int seed, std::string query) override;
};

#endif