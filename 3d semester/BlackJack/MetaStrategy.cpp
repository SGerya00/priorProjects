#include "MetaStrategy.h"
#include "StrategyFactory.h"
#include "CardCounter.h"
#include "Reactive.h"

namespace {
    constexpr char NAME[] = "metastrategy";
    
    std::unique_ptr<Strategy> creator_function() {
        std::string name = NAME;
        std::unique_ptr<Strategy> for_return = std::make_unique<MetaStrategy>(name);
        return for_return;
    }

    bool temp = Factory::get_instance()->register_creator_function(NAME, creator_function);
}

MetaStrategy::MetaStrategy(const std::string& name) : Strategy(name){
    p_strategy1 = std::make_unique<CardCounter>(name);
    p_strategy2 = std::make_unique<Reactive>(name);
}

bool MetaStrategy::make_decision() {
    bool advice = consult_strategies();
    return advice;
}

void MetaStrategy::receive_card(Cards::card new_card) {
    Strategy::receive_card(new_card);
    p_strategy1->receive_card(new_card);
    p_strategy2->receive_card(new_card);
}

void MetaStrategy::set_opponents_first_card(const Strategy& opponent) {
    Strategy::set_opponents_first_card(opponent);
    p_strategy1->set_opponents_first_card(opponent);
    p_strategy2->set_opponents_first_card(opponent);
}

void MetaStrategy::reset_strategy() {
    Strategy::reset_strategy();
    p_strategy1->reset_strategy();
    p_strategy2->reset_strategy();
}

bool MetaStrategy::consult_strategies() {
    bool advice1 = p_strategy1->make_decision();
    bool advice2 = p_strategy2->make_decision();
    if (advice1 || advice2) {
        return true;
    } else {
        return false;
    }
}

void MetaStrategy::read_from_config(const std::string &config_dirname) {
    p_strategy1->read_from_config(config_dirname);
    p_strategy2->read_from_config(config_dirname);
}

MetaStrategy::~MetaStrategy() {
    p_strategy1.reset(); //replace managed object with nothing
    p_strategy2.reset(); //replace managed object with nothing
}
