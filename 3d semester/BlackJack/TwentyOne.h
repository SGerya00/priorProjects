#pragma once
#ifndef BLACKJACK_TWENTYONE_H
#define BLACKJACK_TWENTYONE_H

#include "Strategy.h"

class TwentyOne : public Strategy {
public:
    explicit TwentyOne(const std::string& name);

    bool make_decision() override;

    ~TwentyOne() = default;
};

#endif //BLACKJACK_TWENTYONE_H
