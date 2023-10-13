#pragma once
#ifndef BLACKJACK_DECK_H
#define BLACKJACK_DECK_H

#include <random>
#include <array>

#include "Cards_nominal.h"

class Deck {
public:
    explicit Deck(size_t n_o_decks = 0);
    //creates deck with given n_o_decks placed together

    Deck(const Deck&) = delete;

    Deck& operator=(const Deck&) = delete;

    ~Deck() = default;
    //-------------------------------------

    unsigned int generate_random_number_deck_specific();

    Cards::card give_card(unsigned int random_number);
    //returns a random card (n_o_decks == 0)
    //pops a random card from a deck consisting of n*52 cards (n_o_decks != 0)

    void restore_deck();
    //resets number of each card value back to 4 * n_o_decks

private:
    std::mt19937 rd;
    unsigned int n_o_decks_ = 0;
    unsigned int total_cards_pulled = 0;
    std::array< std::pair<Cards::card, size_t>, 13 > deck;
};

#endif //BLACKJACK_DECK_H
