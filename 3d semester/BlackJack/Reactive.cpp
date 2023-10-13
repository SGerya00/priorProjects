#include "Reactive.h"

#include <fstream>

#include "StrategyFactory.h"

namespace {
    constexpr char NAME[] = "reactive";

    std::unique_ptr<Strategy> creator_function() {
        std::string name = NAME;
        std::unique_ptr<Strategy> for_return = std::make_unique<Reactive>(name);
        return for_return;
    }

    bool temp = Factory::get_instance()->register_creator_function(NAME, creator_function);
}

Reactive::Reactive(const std::string& name) : Strategy(name) {
    for (Cards::card i = 0; i < 10; i++) {
        strategy_table[i].first = (i + 2);
        strategy_table[i].second = true;
    } // 2 3 4 5 6 7 8 9 10 11
} //creates an array of 10 elements which will be consulted by the strategy for decision making

bool Reactive::make_decision() {
    Cards::score current_score = get_score();
    if (current_score >= enough) {
        return false;
    }
    bool decision = consult_strategy_table();
    return decision;
}

bool Reactive::consult_strategy_table() {
    Cards::card opps_card = get_opponents_first_card();
    for (Cards::card i = 0; i < 10; i++) {
        if (opps_card == strategy_table[i].first) {
            return strategy_table[i].second;
        }
    }
    //assert(false);
    return false;
}

void Reactive::read_from_config(const std::string &config_dirname) {
    std::string filename = config_dirname;
    filename.append("/").append(NAME).append(".txt");
    std::ifstream config_file;
    config_file.exceptions(std::ifstream::failbit | std::ifstream::badbit); //a requirement
    try {
        config_file.open(filename); //if it was not opened -> failbit exception will trigger
    }
    catch(std::ifstream::failure& failure) {
        throw std::runtime_error("Unable to open configuration file");
    }
    Cards::score enough_from_file = 0;
    config_file >> enough_from_file;
    if (BLACKJACK >= enough_from_file) {
        enough = enough_from_file;
    } //only change enough if change is valid

    for (size_t i = 0; i < 10; i++) {
        Cards::card read_card = 0;
        config_file >> read_card;
        if ((2 > read_card) || (11 < read_card)) {
            throw std::runtime_error
            ("Bad config file: one of the card values in unexpected (less than 2 or more than 11");
        }
        if (read_card != strategy_table[i].first) {
            throw std::runtime_error("Bad config file: card values lines in disorder / card value line is missing");
        }
        int decision = -1;
        config_file >> decision;
        if ((0 != decision) && (1 != decision)) {
            throw std::runtime_error("Bad config file: for one of the card values a decision is neither {1} nor {0}");
        }
        strategy_table[i].second = static_cast<bool>(decision);
    }
    //config_file.close(); is not needed
}

//Reactive::~Reactive() {} is default
