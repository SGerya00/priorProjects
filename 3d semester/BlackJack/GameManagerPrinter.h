#pragma once
#ifndef BLACKJACK_GAMEMANAGERPRINTER_H
#define BLACKJACK_GAMEMANAGERPRINTER_H

#include <memory>
#include <vector>

#include "Strategy.h"
#include "Cards_nominal.h"

constexpr char QUIT[] = "quit";

class GameManagerPrinter {
public:
    GameManagerPrinter() = default;

    GameManagerPrinter(const GameManagerPrinter&) = delete;

    GameManagerPrinter& operator=(const GameManagerPrinter&) = delete;

    ~GameManagerPrinter() = default;
    //-----------------------------------------------------------------

    void print_initial_state_DEFAULT(const std::string& _1st_name, const std::string& _2nd_name,
                                     Cards::card _1st_1st_c, Cards::card _2nd_1st_c,
                                     Cards::score _1st_init_score, Cards::score _2nd_init_score);

    void print_in_game_state_DEFAULT(const std::string& _1st_name, const std::string& _2nd_name,
                                     bool d_1st, bool d_2nd,
                                     Cards::card _1st_another_c, Cards::card _2nd_another_c);

    void print_end_game_state_DEFAULT(const std::string& _1st_name, const std::string& _2nd_name,
                                      short int winner, Cards::score _1st_score,
                                      Cards::score _2nd_score, std::string* quit);

    void print_greating_FAST();

    void print_end_game_state_one_round(const std::string& _1st_name, const std::string& _2nd_name,
                                        short int winner, Cards::score _1st_score, Cards::score _2nd_score);

    void print_greating_TOURNAMENT();

    void print_end_game_table_TOURNAMENT(const std::vector< std::unique_ptr<Strategy> >& strategies,
                                         std::vector<size_t>& scores);
};

#endif //BLACKJACK_GAMEMANAGERPRINTER_H
