#pragma once
#ifndef BLACKJACK_METASTRATEGY_H
#define BLACKJACK_METASTRATEGY_H

#include <memory>

#include "Strategy.h"
#include "Cards_nominal.h"

class MetaStrategy : public Strategy {
public:
    explicit MetaStrategy(const std::string& name);

    bool make_decision() override;

    void receive_card(Cards::card new_card) override;

    void set_opponents_first_card(const Strategy& opponent) override;

    void reset_strategy() override;

    void read_from_config(const std::string& config_dirname) override;
    //may throw exceptions

    ~MetaStrategy();

private:
    bool consult_strategies();

    std::unique_ptr<Strategy> p_strategy1;
    std::unique_ptr<Strategy> p_strategy2;
};

#endif //BLACKJACK_METASTRATEGY_H
