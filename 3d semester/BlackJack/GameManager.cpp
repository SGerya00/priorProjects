#include "GameManager.h"

GameManager::GameManager(const std::string& game_mode, 
                         std::vector< std::unique_ptr<Strategy> >& vector_of_strategies) :
                            vector_of_strategies_(vector_of_strategies) {
    game_mode_ = game_mode;
    game_deck = std::make_unique<Deck>(0);
}

void GameManager::play_game() {
    if (DEFAULT == game_mode_) {
        play_game_DEFAULT();
    } else if (FAST == game_mode_) {
        play_game_FAST();
    } else if (TOURNAMENT == game_mode_) {
        play_game_TOURNAMENT();
    } else {
        throw std::invalid_argument("Unrecognized game mode name");
    }
} //throws exception if unrecognized game mode

Cards::card GameManager::get_card_from_deck() {
    unsigned int random_number = game_deck->generate_random_number_deck_specific();
    Cards::card new_card = game_deck->give_card(random_number);
    return new_card;
}

GameManager::~GameManager() {
    game_deck.reset();
}

void GameManager::play_game_DEFAULT() {
    while (true) {
        //put all cards back ----------------------------------------------------
        game_deck->restore_deck();
        //get competitors -------------------------------------------------------
        Strategy* _1st_strategy = vector_of_strategies_.at(0).get();
        Strategy* _2nd_strategy = vector_of_strategies_.at(1).get();
        //reset competitors -----------------------------------------------------
        _1st_strategy->reset_strategy();
        _2nd_strategy->reset_strategy();
        //draw first card -------------------------------------------------------
        Cards::card _1st_strategy_1st_card = get_card_from_deck();
        Cards::card _2nd_strategy_1st_card = get_card_from_deck();
        _1st_strategy->receive_card(_1st_strategy_1st_card);
        _2nd_strategy->receive_card(_2nd_strategy_1st_card);
        //draw second card
        _1st_strategy->receive_card(get_card_from_deck());
        _2nd_strategy->receive_card(get_card_from_deck());
        //initial score ---------------------------------------------------------
        short int _1st_score = _1st_strategy->get_score();
        short int _2nd_score = _2nd_strategy->get_score();
        //present initial state -------------------------------------------------
        helper.print_initial_state_DEFAULT(_1st_strategy->get_name(), _2nd_strategy->get_name(),
                                           _1st_strategy_1st_card, _2nd_strategy_1st_card, _1st_score, _2nd_score);
        //look at opponent's first card -----------------------------------------
        _1st_strategy->set_opponents_first_card(*_2nd_strategy);
        _2nd_strategy->set_opponents_first_card(*_1st_strategy);
        //the game itself -------------------------------------------------------
        while (true) {
            bool d_1st = _1st_strategy->make_decision();
            Cards::card _1st_another_card = 0;
            if (true == d_1st) {
                _1st_another_card = get_card_from_deck();
                _1st_strategy->receive_card(_1st_another_card);
            } //else nothing
            bool d_2nd = _2nd_strategy->make_decision();
            Cards::card _2nd_another_card = 0;
            if (true == d_2nd) {
                _2nd_another_card = get_card_from_deck();
                _2nd_strategy->receive_card(_2nd_another_card);
            } //else nothing
            helper.print_in_game_state_DEFAULT(_1st_strategy->get_name(), _2nd_strategy->get_name(),
                                               d_1st, d_2nd, _1st_another_card, _2nd_another_card);
            if ((false == d_1st) && (false == d_2nd)) {
                break;
            }
        }
        //results ---------------------------------------------------------------
        _1st_score = _1st_strategy->get_score();
        _2nd_score = _2nd_strategy->get_score();
        if (_1st_score > BLACKJACK) { _1st_score = 0; }
        if (_2nd_score > BLACKJACK) { _2nd_score = 0; }
        auto winner = static_cast<short int>(_2nd_score > _1st_score) + 1;
        if (_1st_score == _2nd_score) { winner = 0; }
        std::string quit;
        helper.print_end_game_state_DEFAULT(_1st_strategy->get_name(), _2nd_strategy->get_name(),
                                            winner, _1st_strategy->get_score(), _2nd_strategy->get_score(), &quit);
        if (QUIT == quit) {
            break;
        }
    }
}

void GameManager::play_game_FAST() {
    helper.print_greating_FAST();
    //get competitors -------------------------------------------------------
    Strategy* _1st_strategy = vector_of_strategies_.at(0).get();
    Strategy* _2nd_strategy = vector_of_strategies_.at(1).get();
    //play game (and print results) -----------------------------------------
    short int winner = play_game_one_round(*_1st_strategy, *_2nd_strategy);
}

void GameManager::play_game_TOURNAMENT() {
    size_t vectors_size = vector_of_strategies_.size();
    std::vector<size_t> scores;
    for (size_t i = 0; i < vectors_size; i++) {
        scores.push_back(0);
    }
    helper.print_greating_TOURNAMENT();
    for (size_t i = 0; i < vectors_size - 1; i++) {
        Strategy* participant1 = vector_of_strategies_.at(i).get();
        for (size_t j = i + 1; j < vectors_size; j++) {
            Strategy* participant2 = vector_of_strategies_.at(j).get();
            short int winner = play_game_one_round(*participant1, *participant2);
            if (1 == winner) {
                scores.at(i) += 1;
            } else if (2 == winner) {
                scores.at(j) += 1;
            } //else nothing
        }
    }
    helper.print_end_game_table_TOURNAMENT(vector_of_strategies_, scores);
}

short int GameManager::play_game_one_round(Strategy& _1st_strategy, Strategy& _2nd_strategy) {
    //put all cards back ----------------------------------------------------
    game_deck->restore_deck();
    //reset competitors -----------------------------------------------------
    _1st_strategy.reset_strategy();
    _2nd_strategy.reset_strategy();
    //draw first card -------------------------------------------------------
    Cards::card _1st_strategy_1st_card = get_card_from_deck();
    Cards::card _2nd_strategy_1st_card = get_card_from_deck();
    _1st_strategy.receive_card(_1st_strategy_1st_card);
    _2nd_strategy.receive_card(_2nd_strategy_1st_card);
    //draw second card
    _1st_strategy.receive_card(get_card_from_deck());
    _2nd_strategy.receive_card(get_card_from_deck());
    //look at opponent's first card -----------------------------------------
    _1st_strategy.set_opponents_first_card(_2nd_strategy);
    _2nd_strategy.set_opponents_first_card(_1st_strategy);
    //the game itself -------------------------------------------------------
    while (true) {
        bool d_1st = _1st_strategy.make_decision();
        if (true == d_1st) {
            _1st_strategy.receive_card(get_card_from_deck());
        } //else nothing
        bool d_2nd = _2nd_strategy.make_decision();
        if (true == d_2nd) {
            _2nd_strategy.receive_card(get_card_from_deck());
        } //else nothing
        if ((false == d_1st) && (false == d_2nd)) {
            break;
        }
    }
    short int _1st_score = _1st_strategy.get_score();
    short int _2nd_score = _2nd_strategy.get_score();
    if (_1st_score > BLACKJACK) { _1st_score = 0; }
    if (_2nd_score > BLACKJACK) { _2nd_score = 0; }
    auto winner = static_cast<short int>((_2nd_score > _1st_score) + 1);
    if (_1st_score == _2nd_score) { winner = 0; }
    helper.print_end_game_state_one_round(_1st_strategy.get_name(), _2nd_strategy.get_name(),
                                          winner, _1st_strategy.get_score(), _2nd_strategy.get_score());
    return winner;
}
//0 for draw, 1 for 1st, 2 for 2nd, prints results of one game (scores and winner)
