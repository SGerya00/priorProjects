#pragma once
#ifndef BLACKJACK_REACTIVE_H
#define BLACKJACK_REACTIVE_H

#include <array>

#include "Strategy.h"

class Reactive : public Strategy {
public:
    explicit Reactive(const std::string& name);

    bool make_decision() override;

    void read_from_config(const std::string& config_dirname) override;
    //may throw exceptions

    ~Reactive() = default;

private:
    bool consult_strategy_table();
    Cards::score enough = 19; //a value which the strategy stands on, 19 by default
    std::array< std::pair<Cards::card, bool>, 10 > strategy_table;
};


#endif //BLACKJACK_REACTIVE_H
