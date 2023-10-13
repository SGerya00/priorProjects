#pragma once
#ifndef BLACKJACK_STRATEGYFACTORY_H
#define BLACKJACK_STRATEGYFACTORY_H

#include <map>
#include <memory>

class Strategy; //forward declaration

class Factory {
public:
    static Factory* get_instance();

    std::unique_ptr<Strategy> create_strategy_by_id(const std::string& id);
    //throws exception if id not registered

    bool register_creator_function(const std::string& id, std::unique_ptr<Strategy> (*creator_function)());

private:
    std::map<std::string, std::unique_ptr<Strategy> (*)()> creator_functions_map;
};

#endif //BLACKJACK_STRATEGYFACTORY_H
