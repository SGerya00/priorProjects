#include "gtest/gtest.h"

#include "Strategy.h"
#include "TwentyOne.h"
#include "StandOn17.h"
#include "TwoIsEnough.h"
#include "CardCounter.h"
#include "Reactive.h"
#include "Deck.h"

TEST(StrategyMethods, TwentyOneTestAces) {
    TwentyOne strat1("twentyone");

    strat1.receive_card(11);
    strat1.receive_card(11);

    bool decision = true;
    while(decision) {
        decision = strat1.make_decision();
        if (decision) {
            strat1.receive_card(11);
        }
    }
    size_t score = strat1.get_score();
    EXPECT_EQ(21, score);
}

TEST(StrategyMethods, StandOn17TestAces) {
    StandOn17 strat1("standon17");

    strat1.receive_card(11);
    strat1.receive_card(11);

    bool decision = true;
    while(decision) {
        decision = strat1.make_decision();
        if (decision) {
            strat1.receive_card(11);
        }
    }
    size_t score = strat1.get_score();
    EXPECT_EQ(17, score);
}

TEST(StrategyMethods, TwoIsEnoughTestAces) {
    TwoIsEnough strat1("twoisenough");

    strat1.receive_card(11);
    strat1.receive_card(11);

    bool decision = true;
    while(decision) {
        decision = strat1.make_decision();
        EXPECT_FALSE(decision);
        if (decision) {
            strat1.receive_card(11);
        }
    }
    size_t score = strat1.get_score();
    EXPECT_EQ(12, score);
}

TEST(StrategyMethods, CardCounterTestAces) {
    CardCounter strat1("cardcounter");
    std::string config_file_dir = "C:\\Users\\XxX_SaveMe_XxX\\Desktop\\configs";
    strat1.read_from_config(config_file_dir);

    strat1.receive_card(11);
    strat1.receive_card(11);

    bool decision = true;
    while(decision) {
        decision = strat1.make_decision();
        if (decision) {
            strat1.receive_card(11);
        }
    }
    size_t score = strat1.get_score();
    EXPECT_EQ(16, score);
}

TEST(StrategyMethods, ReactiveTestAces) {
    Reactive strat1("reactive");
    std::string config_file_dir = "C:\\Users\\XxX_SaveMe_XxX\\Desktop\\configs";
    strat1.read_from_config(config_file_dir);

    strat1.receive_card(11);
    strat1.receive_card(11);

    TwentyOne strat_opponent("twentyone");

    for (size_t i = 0; i < 9; i++) {
        strat_opponent.receive_card(i + 2);
        strat1.set_opponents_first_card(strat_opponent);
        bool decision = strat1.make_decision();
        if ((i >= 2) && (i <= 4)) {
            EXPECT_FALSE(decision);
        } else {
            EXPECT_TRUE(decision);
        }
        strat_opponent.reset_strategy();
    }

    strat_opponent.receive_card(11);

    bool decision = true;
    while(decision) {
        decision = strat1.make_decision();
        if (decision) {
            strat1.receive_card(11);
        }
    }
    size_t score = strat1.get_score();
    EXPECT_EQ(18, score); //from config
}

TEST(DeckMethods, GiveCardTest) {
    short int card_got;

    Deck new_deck(0); //basic 0 decks deck

    for (size_t i = 0; i < 200; i++) {
        unsigned int random_number = new_deck.generate_random_number_deck_specific();
        EXPECT_NO_THROW(card_got = new_deck.give_card(random_number));
    }

    Deck new_deck2(1); //standard 52 card deck

    for (size_t i = 0; i < 52; i++) {
        unsigned int random_number = 52 - i;
        EXPECT_NO_THROW(card_got = new_deck2.give_card(random_number));
        if (i < 4) {
            EXPECT_EQ(11, card_got);
        }
    }
    for (size_t i = 52; i < 100; i++) {
        unsigned int random_number = 1; //whatever, it does not matter, since no cards are left
        EXPECT_ANY_THROW(card_got = new_deck2.give_card(random_number));
    }

    Deck new_deck3(4); //deck composed of 4 identical 52 card decks

    for (size_t i = 0; i < 52 * 4; i++) {
        unsigned int random_number = 52 * 4 - i;
        EXPECT_NO_THROW(card_got = new_deck3.give_card(random_number));
        if (i < 4 * 4) {
            EXPECT_EQ(11, card_got);
        } else if (i < 5 * 4 * 4) {
            EXPECT_EQ(10, card_got);
        }
    }
    for (size_t i = 52 * 4; i < 250; i++) {
        unsigned int random_number = 16; //again, whatever
        EXPECT_ANY_THROW(card_got = new_deck3.give_card(random_number));
    }
}