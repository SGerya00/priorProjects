#include "TwentyOne.h"

#include "StrategyFactory.h"

namespace {
    constexpr char NAME[] = "twentyone";

    std::unique_ptr<Strategy> creator_function() {
        std::string name = NAME;
        std::unique_ptr<Strategy> for_return = std::make_unique<TwentyOne>(name);
        return for_return;
    }

    bool temp = Factory::get_instance()->register_creator_function(NAME, creator_function);
}

TwentyOne::TwentyOne(const std::string &name) : Strategy(name) {}

bool TwentyOne::make_decision() {
    Cards::score score = get_score();
    return (score < BLACKJACK);
}

//TwentyOne::~TwentyOne() {} is default
