#include "StrategyFactory.h"

#include "Strategy.h" //otherwise iterator->second() would not work

Factory* Factory::get_instance() {
    static Factory factory;
    return &factory;
}

std::unique_ptr<Strategy> Factory::create_strategy_by_id(const std::string& id) {
    auto iterator = creator_functions_map.find(id);
    if (iterator == creator_functions_map.end()) {
        throw std::invalid_argument("Unrecognized strategy ID"); //exception
    }
    return iterator->second();
} //throws exception if id not registered

bool Factory::register_creator_function(const std::string& id, std::unique_ptr<Strategy> (*creator_function)()) {
    creator_functions_map[id] = creator_function;
    return true;
}