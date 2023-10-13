#pragma once
#ifndef BLACKJACK_CARDCOUNTER_H
#define BLACKJACK_CARDCOUNTER_H

#include "Strategy.h"
#include "Cards_nominal.h"

class CardCounter : public Strategy {
public:
    explicit CardCounter(const std::string& name) : Strategy(name) {}

    bool make_decision() override;

    void set_opponents_first_card(const Strategy& opponent) override;

    void receive_card(Cards::card new_card) override;

    void read_from_config(const std::string& config_dirname) override;
    //may throw exceptions

    ~CardCounter() = default;

private:
    short int deck_score = 0; //>0 if likely to get >= than criteria, <0 otherwise
    Cards::card criteria = Cards::EIGHT; //default value
};

#endif //BLACKJACK_CARDCOUNTER_H
