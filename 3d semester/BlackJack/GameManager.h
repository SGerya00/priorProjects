#pragma once
#ifndef BLACKJACK_GAMEMANAGER_H
#define BLACKJACK_GAMEMANAGER_H

#include <random>
#include <memory>

#include "GameManagerPrinter.h"
#include "Deck.h"

constexpr char DEFAULT[] = "detailed";
constexpr char FAST[] = "fast";
constexpr char TOURNAMENT[] = "tournament";

class GameManager {
public:
    GameManager(const std::string& game_mode, std::vector< std::unique_ptr<Strategy> >& vector_of_strategies);
    //to be constructed, GameManager needs vector_of_strategies to be alive as long as GameManager itself
    //is alive, so vector_of_strategies should be destroyed only after GameManager is destroyed

    GameManager(const GameManager&) = delete;

    GameManager& operator=(const GameManager&) = delete;

    ~GameManager();
    //---------------------------------------------------

    void play_game(); //throws exception if unrecognized game mode

private:
    Cards::card get_card_from_deck();

    void play_game_DEFAULT();

    void play_game_FAST();

    void play_game_TOURNAMENT();

    short int play_game_one_round(Strategy& _1st_strategy, Strategy& _2nd_strategy);
    //0 for draw, 1 for 1st, 2 for 2nd

    GameManagerPrinter helper;
    std::unique_ptr<Deck> game_deck;
    std::string game_mode_;
    std::vector< std::unique_ptr<Strategy> >& vector_of_strategies_;
};

#endif //BLACKJACK_GAMEMANAGER_H
