#pragma once
#ifndef BLACKJACK_TWOISENOUGH_H
#define BLACKJACK_TWOISENOUGH_H

#include "Strategy.h"

class TwoIsEnough : public Strategy{
public:
    explicit TwoIsEnough(const std::string& name);

    bool make_decision() override;

    ~TwoIsEnough() = default;
};

#endif //BLACKJACK_TWOISENOUGH_H
