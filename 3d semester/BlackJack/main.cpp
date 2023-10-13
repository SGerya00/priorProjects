#include <iostream>
#include <vector>
#include <cassert>

#include "StrategyFactory.h"
#include "Strategy.h"
#include "GameManager.h"

//-----------------------------------------------------------------------------
//#define GOOGLE_TEST_ON

#ifdef GOOGLE_TEST_ON
#include "gtest/gtest.h"
#endif
//-----------------------------------------------------------------------------

int check_input(int argc, char **argv,
                std::vector< std::string >* strategy_names, std::string* gamemode, std::string* config_dir) {
    assert(nullptr != strategy_names);
    assert(nullptr != gamemode);
    assert(nullptr != config_dir);                
    if (argc < 3) {
        std::cerr << "Not enough arguments: 2 or more expected, given " << argc - 1 << std::endl;
        return -1;
    }
    //set defaults -----------------------------------------------------------------------------------------------------
    (*gamemode) = DEFAULT;
    (*config_dir) = ""; //an empty string
    //read names of participating strategies ---------------------------------------------------------------------------
    std::string current_word;
    size_t iterator = 1;
    while (true) {
        if (iterator == argc) {
            break;
        }
        current_word = argv[iterator];
        if ('-' == current_word[0]) {
            break;
        } //current word is assumed to be a flag, no more strategy names
        strategy_names->push_back(current_word);
        iterator += 1;
    } //after loop iterator = number of the first argument that is not a strategy name or iterator = argc
    if (2 > strategy_names->size()) {
        throw std::runtime_error("Less than 2 potential strategy names are given");
    }
    //set defaults (again) ---------------------------------------------------------------------------------------------
    if (3 <= strategy_names->size()) {
        (*gamemode) = TOURNAMENT; //another default setting
    }
    if (iterator == argc) {
        return 0;
    } //no special flags, just strategy names
    //more arguments to read, assumed flags ----------------------------------------------------------------------------
    std::string prefix_expected1 = "--mode=[";
    std::string prefix_expected2 = "--configs=[";
    std::string prefix1 = current_word.substr(0, prefix_expected1.length());
    std::string prefix2 = current_word.substr(0, prefix_expected2.length());
    if (prefix1 == prefix_expected1) {
        std::string mode = current_word.substr(prefix_expected1.length(),
                                               current_word.length() - prefix_expected1.length() - 1);
        if (']' != current_word.back()) {
            throw std::runtime_error("Bad syntax in format string: last symbol expected to be \']\'"
                                     "(correct syntax is \"--mode=[*game mode name*]\")");
        }
        (*gamemode) = mode;
        iterator += 1; //we read "--mode=[...]" but we may still need to read "--configs<...>"
    } else if (prefix2 == prefix_expected2) {
        std::string directory = current_word.substr(prefix_expected2.length(),
                                                    current_word.length() - prefix_expected2.length() - 1);
        if (']' != current_word.back()) {
            throw std::runtime_error("Bad syntax in format string: last symbol expected to be \']\'"
                                     "(correct syntax is \"--configs=[*directory name*]\")");
        }
        (*config_dir) = directory;
        return 0; //nothing is supposed to be after "--configs<...>" so we won't even look there
    } else {
        throw std::runtime_error("Unrecognized flag");
    }
    if (iterator == argc) {
        return 0;
    } //no config directory flag, just strategy names and game mode
    //one flag read, assumed 1 more exists -----------------------------------------------------------------------------
    current_word = argv[iterator];
    prefix2 = current_word.substr(0, prefix_expected2.length());
    if (prefix2 == prefix_expected2) {
        std::string directory = current_word.substr(prefix_expected2.length(),
                                                    current_word.length() - prefix_expected2.length() - 1);
        if (']' != current_word.back()) {
            throw std::runtime_error("Bad syntax in format string: last symbol expected to be \']\'"
                                     "(correct syntax is \"--configs=[*directory name*]\")");
        }
        (*config_dir) = directory;
        return 0; //nothing is supposed to be after "--configs<...>" so we won't even look there
    } else {
        throw std::runtime_error("Unrecognized flag");
    }
}

//twoisenough standon17 twentyone cardcounter reactive metastrategy
int main(int argc, char **argv) {
#ifndef GOOGLE_TEST_ON
    std::vector< std::string > vector_of_strategy_names;
    std::string gamemode;
    std::string config_dir;
    try {
        if (0 != check_input(argc, argv, &vector_of_strategy_names, &gamemode, &config_dir)) {
            return -1;
        }
    }
    catch(std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return -1;
    }

    std::vector< std::unique_ptr<Strategy> > strategies;
    for (std::string& name : vector_of_strategy_names) {
        try {
            strategies.push_back(Factory::get_instance()->create_strategy_by_id(name));
            //factory may throw std::invalid_argument if no strategy with given id
        }
        catch(std::invalid_argument& error) {
            std::cerr << "Error: " << error.what() << std::endl;
            continue;
        }
        try {
            strategies[strategies.size() - 1]->read_from_config(config_dir);
            //reading from configs may also throw exceptions
        }
        catch(std::runtime_error& error) {
            std::cerr << "Error: " << error.what() << std::endl;
            strategies.pop_back();
            continue;
        }
    }
    if (strategies.size() < 2) {
        std::cerr << "Error: not enough (<2) strategies for a game, terminating" << std::endl;
        return -1;
    }
    //-----------------------------------------------------------------------
    GameManager GM(gamemode, strategies);
    //set up random device, create deck and connect strategies --------------
    try {
        GM.play_game();
    }
    catch(std::invalid_argument& error) {
        std::cerr << error.what() << std::endl;
        return -1;
    }
    //-----------------------------------------------------------------------
    return 0;
#else
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
}
