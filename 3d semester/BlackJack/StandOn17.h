#pragma once
#ifndef BLACKJACK_STANDON17_H
#define BLACKJACK_STANDON17_H

#include "Strategy.h"

class StandOn17 : public Strategy {
public:
    explicit StandOn17(const std::string& name);

    bool make_decision() override;

    ~StandOn17() = default;
};

#endif //BLACKJACK_STANDON17_H
