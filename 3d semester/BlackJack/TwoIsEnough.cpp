#include "TwoIsEnough.h"

#include "StrategyFactory.h"

namespace {
    constexpr char NAME[] = "twoisenough";

    std::unique_ptr<Strategy> creator_function() {
        std::string name = NAME;
        std::unique_ptr<Strategy> for_return  = std::make_unique<TwoIsEnough>(name);
        return for_return;
    }

    bool temp = Factory::get_instance()->register_creator_function(NAME, creator_function);
}

TwoIsEnough::TwoIsEnough(const std::string &name) : Strategy(name) {}

bool TwoIsEnough::make_decision() {
    return false;
}

//TwoIsEnough::~TwoIsEnough() {} is default
