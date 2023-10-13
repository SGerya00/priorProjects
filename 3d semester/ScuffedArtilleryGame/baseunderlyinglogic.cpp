#include "BaseUnderlyingLogic.h"

//BaseLogic::BaseLogic() {} is default

void BaseLogic::set_ptr_to_foes_logic(BaseLogic& link_to_foes_logic) {
    foes_logic = &link_to_foes_logic;
}

void BaseLogic::send_attack_vector() {
    foes_logic->receive_attack_vector(own_attack_vector);
    own_attack_vector.clear();
}

void BaseLogic::receive_attack_vector(const coords_vector& attack_vector) {
    foes_attack_vector.clear();
    for (size_t i = 0; i < attack_vector.size(); i++) {
        foes_attack_vector.push_back({attack_vector[i], false});
    }
    react_to_attack();
}

const coords_bool_vector& BaseLogic::pass_processed_foes_attack_vector_to_middleman() const {
    return foes_attack_vector;
}

void BaseLogic::react_to_attack() {
    for (size_t i = 0; i < foes_attack_vector.size(); i++) {
        for (size_t j = 0; j < coords_of_active_guns.size(); j++) {
            if (foes_attack_vector[i].first == coords_of_active_guns[j]) {
                foes_attack_vector[i].second = true;
                coords_of_active_guns.erase(coords_of_active_guns.begin() + j);
            }
        }
    }
    //after this loop, foes_attack_vector has elements set to true/false and
    //some elements in coords_of_active_guns may have been erased if corresponding
    //coordinates in foes_attack_vector are set to true
}

void BaseLogic::set_deployment_number(size_t n) {
    deployment_number = n;
}

void BaseLogic::reset_self() {
    own_attack_vector.clear();
    coords_of_active_guns.clear();
    foes_attack_vector.clear();
    foes_logic = nullptr;
}

//BaseLogic::~BaseLogic() {} is default


