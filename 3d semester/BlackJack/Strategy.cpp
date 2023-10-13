#include "Strategy.h"

Strategy::Strategy(const std::string& strategy_name) {
    name = strategy_name;
}

void Strategy::receive_card(Cards::card new_card) {
    if (0 == score) {
        first_card = new_card;
    }
    score += new_card;
    if (Cards::ACE == new_card) {
        n_o_aces += 1;
    }
    if ((score > BLACKJACK) && (n_o_aces > 0)) {
        n_o_aces -= 1;
        score -= Cards::ACE;
        score += 1;
    }
}

void Strategy::set_opponents_first_card(const Strategy& opponent) {
        opponents_first_card = opponent.present_first_card();
    }

void Strategy::reset_strategy() {
    opponents_first_card = nullptr;
    score = 0;
    n_o_aces = 0;
}

void Strategy::read_from_config(const std::string& config_dirname) {
    //nothing
}

//Strategy::~Strategy() {} is default