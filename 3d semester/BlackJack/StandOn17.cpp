#include "StandOn17.h"
#include "StrategyFactory.h"

namespace {
    constexpr char NAME[] = "standon17";

    std::unique_ptr<Strategy> creator_function() {
        std::string name = NAME;
        std::unique_ptr<Strategy> for_return = std::make_unique<StandOn17>(name);
        return for_return;
    }

    bool temp = Factory::get_instance()->register_creator_function(NAME, creator_function);
}

StandOn17::StandOn17(const std::string &name) : Strategy(name){}

bool StandOn17::make_decision() {
    Cards::score score = get_score();
    return (score < 17); //it's in the name
}

//StandOn17::~StandOn17() {} is default
