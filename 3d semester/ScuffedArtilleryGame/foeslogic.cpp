#include "foeslogic.h"

#include <chrono> //for rng seed

#include "SIZES_NMSPC.h"

FoesLogic::FoesLogic() : random_device( std::chrono::system_clock::now().time_since_epoch().count() ){
    //nothing except random_device being made with time as seed
}

void FoesLogic::make_attack_vector() {
    size_t n_o_active_guns = get_n_o_active_guns();
    if (0 == n_o_active_guns) {
        own_attack_vector.clear();
        return;
    }
    size_t good_coords_generated_so_far = 0;
    while(true) {
        int number_got1 = make_random_number();
        int number_got2 = make_random_number();
        coords_pair generated_pair_of_coords = {number_got1, number_got2};
        bool dublicate_flag = false;
        for (size_t j = 0; j < already_hit.size(); j++) {
            if (generated_pair_of_coords == already_hit[j]) {
                dublicate_flag = true;
            }
        }
        if (true == dublicate_flag) {
            continue; //if already hit that square, skip it
        } else {
            bool dublicate_flag = false;
            for (size_t i = 0; i < own_attack_vector.size(); i++) {
                if (own_attack_vector[i] == generated_pair_of_coords) {
                    dublicate_flag = true;
                }
            }
            if (true == dublicate_flag) {
                continue; //if already included that square in attack vector, skip it
            } else {
                good_coords_generated_so_far += 1;
                own_attack_vector.push_back(generated_pair_of_coords);
                if (n_o_active_guns == good_coords_generated_so_far) {
                    break; //if did not yet encounter that square, include it in attack vector
                }
            }
        }
    }
}

void FoesLogic::deploy_guns() {
    size_t good_coords_generated_so_far = 0;
    while(true) {
        int number_got1 = make_random_number();
        int number_got2 = make_random_number();
        coords_pair generated_pair_of_coords = {number_got1, number_got2};
        bool dublicate_flag = false;
        for (size_t j = 0; j < coords_of_active_guns.size(); j++) {
            int a = abs(coords_of_active_guns[j].first - number_got1);
            int b = abs(coords_of_active_guns[j].second - number_got2);
            if ((a <= 1) && (b <= 1)) {
                dublicate_flag = true;
            }
        }
        if (true == dublicate_flag) {
            continue; //if already selected that square (+ it's vicinity), skip it
        } else {
            good_coords_generated_so_far += 1;
            coords_of_active_guns.push_back(generated_pair_of_coords);
            if (coords_of_active_guns.size() >= get_deployment_number()) {
                break;
            }
        }
    }
}

void FoesLogic::process_attack_info(const coords_bool_vector& processed_attack_vector) {
    for (size_t i = 0; i < processed_attack_vector.size(); i++) {
        if (false == processed_attack_vector[i].second) {
            already_hit.push_back(processed_attack_vector[i].first);
        } else {
            //need to add vicinity to already_hit
            for (size_t j = 0; j < 9; j++) {
                int first_coord = processed_attack_vector[i].first.first + ((j % 3) - 1);
                int second_coord = processed_attack_vector[i].first.second + ((j / 3) - 1);
                if ((first_coord < 0) || (second_coord < 0) ||
                    (first_coord == SIZES::GRID_SIZE_IN_SQUARES) || (second_coord == SIZES::GRID_SIZE_IN_SQUARES)) {
                    continue;
                    //inappropriate coordinates, do not add
                } else {
                    coords_pair to_add{first_coord, second_coord};
                    bool dublicate_flag = false;
                    for (size_t t = 0; t < already_hit.size(); t++) {
                        if (to_add == already_hit[t]) {
                            dublicate_flag = true;
                            break;
                        }
                    }
                    if (true == dublicate_flag) {
                        continue;
                    } else {
                        already_hit.push_back(to_add);
                    }
                }
            }
        }
    }
}

void FoesLogic::reset_self() {
    BaseLogic::reset_self();
    already_hit.clear();
}

//private
int FoesLogic::make_random_number() {
    int number_got = 0;
    std::uniform_int_distribution<int> dist(0, SIZES::GRID_SIZE_IN_SQUARES - 1);
    number_got = dist(random_device);
    return number_got;
}
