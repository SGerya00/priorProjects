#include "ownlogic.h"

void OwnLogic::make_attack_vector(const coords_vector& v_o_hit_markers) {
    for (size_t i = 0; i < v_o_hit_markers.size(); i++) {
        own_attack_vector.push_back(v_o_hit_markers[i]);
    }
}

void OwnLogic::receive_deployment_vector(const coords_vector& deployment_vector) {
    for (size_t i = 0; i < deployment_vector.size(); i++) {
        coords_of_active_guns.push_back(deployment_vector[i]);
    }
}

//OwnLogic::~OwnLogic() {} is default
