#pragma once
#ifndef BLACKJACK_CARDS_NOMINAL_H
#define BLACKJACK_CARDS_NOMINAL_H

namespace Cards {
    typedef short int score;
    typedef short int card;

    enum {
        TWO = 2,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,
        TEN,
        JACK = 10,
        QUEEN = 10,
        KING = 10,
        ACE = 11
    };
}

#endif //BLACKJACK_CARDS_NOMINAL_H
