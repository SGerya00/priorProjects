#pragma once
#ifndef BLACKJACK_STRATEGY_H
#define BLACKJACK_STRATEGY_H

#include <string>

#include "Cards_nominal.h"

constexpr Cards::score BLACKJACK = 21;

class Strategy {
public:
    explicit Strategy(const std::string& name);

    Strategy(const Strategy&) = delete;

    Strategy& operator=(const Strategy&) = delete;

    virtual ~Strategy() = default;
    //--------------------------------------------

    virtual bool make_decision() = 0;

    virtual void receive_card(Cards::card new_card);

    const Cards::card* present_first_card() const {
        return &first_card;
    }

    virtual void set_opponents_first_card(const Strategy& opponent);

    Cards::card get_opponents_first_card() const {
        return *opponents_first_card;
    }

    Cards::score get_score() const {
        return score;
    }

    const std::string& get_name() const {
        return name;
    }

    virtual void reset_strategy();

    virtual void read_from_config(const std::string& config_dirname);
    //children may throw exceptions

private:
    std::string name;
    Cards::card first_card = 0;
    const Cards::card* opponents_first_card = nullptr;
    Cards::score score = 0;
    short int n_o_aces = 0;
};

#endif //BLACKJACK_STRATEGY_H
