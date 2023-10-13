#include "CardCounter.h"

#include <fstream>

#include "StrategyFactory.h"

namespace {
    constexpr char NAME[] = "cardcounter"; //std::string won't do, since this is 17, not 20

    std::unique_ptr<Strategy> creator_function() {
        std::string name = NAME;
        std::unique_ptr<Strategy> for_return = std::make_unique<CardCounter>(name);
        return for_return;
    }

    bool temp = Factory::get_instance()->register_creator_function(NAME, creator_function);
}

bool CardCounter::make_decision() {
    Cards::card anticipated = 0;
    if (deck_score >= 3) {
        anticipated = criteria - 2; //>0
    } else if (deck_score <= -3) {
        anticipated = criteria + 2; //>0
    } else {
        anticipated = criteria;
    }

    Cards::score score = get_score();
    Cards::score projection = score + anticipated;

    return (projection <= BLACKJACK);
}

void CardCounter::receive_card(Cards::card new_card) {
    this->Strategy::receive_card(new_card);
    if (criteria <= new_card) {
        deck_score += 1;
    } else {
        deck_score -= 1;
    }
} //augmented

void CardCounter::set_opponents_first_card(const Strategy& opponent) {
    Strategy::set_opponents_first_card(opponent);
    Cards::card opps_card = get_opponents_first_card();
    if (criteria <= opps_card) {
        deck_score += 1;
    } else {
        deck_score -= 1;
    }
} //augmented

void CardCounter::read_from_config(const std::string& config_dirname) {
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
    Cards::card criteria_from_file;
    config_file >> criteria_from_file;
    if ((1 >= criteria_from_file) || (11 < criteria_from_file)) {
        //nothing
    } else {
        criteria = criteria_from_file;
    } //only changing criteria if change is valid
    //config_file.close(); is not needed
}

//CardCounter::~CardCounter() {} is default
