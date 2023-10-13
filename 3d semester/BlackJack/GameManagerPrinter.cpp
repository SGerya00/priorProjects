#include "GameManagerPrinter.h"

#include <iostream>

void GameManagerPrinter::print_initial_state_DEFAULT(const std::string& _1st_name, const std::string& _2nd_name,
                                                     Cards::card _1st_1st_c, Cards::card _2nd_1st_c,
                                                     Cards::score _1st_init_score, Cards::score _2nd_init_score) {
    std::cout << "\n------DETAILED game------" << std::endl;
    std::cout << "--{" << _1st_name << "} is playing against {" << _2nd_name << "}--" << std::endl;
    std::cout << "--{" << _1st_name << "}'s first card is [" << _1st_1st_c << "]--" << std::endl;
    std::cout << "--{" << _2nd_name << "}'s first card is [" << _2nd_1st_c << "]--" << std::endl;

    std::cout << "--{" << _1st_name << "}'s initial score is [[" << _1st_init_score << "]]--" << std::endl;
    std::cout << "--{" << _2nd_name << "}'s initial score is [[" << _2nd_init_score << "]]--" << std::endl;
}

void GameManagerPrinter::print_in_game_state_DEFAULT(const std::string& _1st_name, const std::string& _2nd_name,
                                                     bool d_1st, bool d_2nd,
                                                     Cards::card _1st_another_c, Cards::card _2nd_another_c) {
    std::cout << "---(new round)---" << std::endl;
    std::cout << "--{" << _1st_name << "} decides to ";
    if (d_1st) {
        std::cout << "draw and gets [" << _1st_another_c << "]--" << std::endl;
    } else {
        std::cout << "stand--" << std::endl;
    }
    std::cout << "--{" << _2nd_name << "} decides to ";
    if (d_2nd) {
        std::cout << "draw and gets [" << _2nd_another_c << "]--" << std::endl;
    } else {
        std::cout << "stand--" << std::endl;
    }
}

void GameManagerPrinter::print_end_game_state_DEFAULT(const std::string& _1st_name, const std::string& _2nd_name,
                                                      short int winner, Cards::score _1st_score,
                                                      Cards::score _2nd_score, std::string* quit) {
    std::cout << "---both strategies stopped drawing---" << std::endl;
    std::cout << "--{" << _1st_name << "} has a score of {" << _1st_score << "}--" << std::endl;
    std::cout << "--{" << _2nd_name << "} has a score of {" << _2nd_score << "}--" << std::endl;
    if (1 == winner) {
        std::cout << "--{" << _1st_name << "} wins!--" << std::endl;
    } else if (2 == winner) {
        std::cout << "--{" << _2nd_name << "} wins!--" << std::endl;
    } else if (0 == winner){
        std::cout << "--it's a draw!--" << std::endl;
    }
    std::cout << "Wish to continue? (type \"" << QUIT << "\" if no)" << std::endl;
    std::cin >> (*quit);
}

void GameManagerPrinter::print_greating_FAST() {
    std::cout << "\n------FAST game------" << std::endl;
}

void GameManagerPrinter::print_end_game_state_one_round(const std::string& _1st_name, const std::string& _2nd_name,
                                                        short int winner, Cards::score _1st_score,
                                                        Cards::score _2nd_score) {
    std::cout << "--(new round)--" << std::endl;
    std::cout << "--{" << _1st_name << "} is playing against {" << _2nd_name << "}--" << std::endl;
    std::cout << "--{" << _1st_name << "}'s score is [[" << _1st_score << "]]--" << std::endl;
    std::cout << "--{" << _2nd_name << "}'s score is [[" << _2nd_score << "]]--" << std::endl;
    if (1 == winner) {
        std::cout << "--{" << _1st_name << "} wins!--" << std::endl;
    } else if (2 == winner) {
        std::cout << "--{" << _2nd_name << "} wins!--" << std::endl;
    } else if (0 == winner){
        std::cout << "--it's a draw!--" << std::endl;
    }
}

void GameManagerPrinter::print_greating_TOURNAMENT() {
    std::cout << "\n------TOURNAMENT game------" << std::endl;
}

void GameManagerPrinter::print_end_game_table_TOURNAMENT(const std::vector< std::unique_ptr<Strategy> >& strategies,
                                                         std::vector<size_t>& scores) {
    std::cout << "\n------TOURNAMENT game results------" << std::endl;
    size_t vectors_size = strategies.size();
    for (size_t i = 0; i < vectors_size; i++) {
        std::cout << "{" << strategies[i]->get_name()
                  << "} has a score of [[" << scores[i] << "]] points" << std::endl;
    }
}