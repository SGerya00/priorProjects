#include "Deck.h"

#include <chrono>

Deck::Deck(size_t n_o_decks) : rd( std::chrono::system_clock::now().time_since_epoch().count() ){
    //idk...
    //p_rd = new std::random_device();
    if (0 != n_o_decks) {
        //p_deck = new std::array<std::pair<Cards::card, size_t>, 13>;
        for (Cards::card i = 0; i < 9; i++) {
            deck[i].first = (i + 2);
        }
        deck[9].first = Cards::JACK;
        deck[10].first = Cards::QUEEN;
        deck[11].first = Cards::KING;
        deck[12].first = Cards::ACE;

        for (Cards::card i = 0; i < 13; i++) {
            (deck[i]).second = 4 * n_o_decks;
        }
        n_o_decks_ = n_o_decks;
    }
//if n_o_decks is 0 -> just give random number
}

//Deck::~Deck() {} is default

unsigned int Deck::generate_random_number_deck_specific() {
    if (0 == n_o_decks_) {
        //-----------------------------------------
        unsigned int number_got = 0;
        std::uniform_int_distribution<unsigned int> dist(0, 9);
        number_got = dist(rd);
        return number_got;
        //-----------------------------------------
    } else {
        //-----------------------------------------
        unsigned int number_got = 0;
        std::uniform_int_distribution<unsigned int> dist(1, (52 * n_o_decks_) - total_cards_pulled);
        number_got = dist(rd);
        return number_got;
        //-----------------------------------------
    }
}

Cards::card Deck::give_card(unsigned int random_number) {
    if (0 == n_o_decks_) {
        auto card_got = static_cast<Cards::card>(random_number + 2);
        return card_got;
    } else {
        //pops a random card from a deck consisting of n*52 cards
        unsigned int number_got = random_number;
        short int iterator = 0;
        while (true) {
            if (iterator >= 13) {
                throw std::runtime_error("No cards left in deck");
            }
            if (deck[iterator].second < number_got){
                number_got -= deck[iterator].second;
                iterator += 1;
            } else {
                deck[iterator].second -= 1;
                break;
            }
        }
        total_cards_pulled += 1;
        return deck[iterator].first;
    }
}

void Deck::restore_deck() {
    if (0 != n_o_decks_) {
        for (Cards::card i = 0; i < 13; i++) {
            (deck[i]).second = 4 * n_o_decks_;
        }
        total_cards_pulled = 0;
    } else {
        //nothing
    }
}
