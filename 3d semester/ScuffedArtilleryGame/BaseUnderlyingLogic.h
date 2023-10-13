#ifndef BASEUNDERLYINGLOGIC_H
#define BASEUNDERLYINGLOGIC_H

#include "BaseGrid.h"

class BaseLogic {

public:
    BaseLogic() = default;

    BaseLogic(const BaseLogic&) = delete;

    BaseLogic& operator=(const BaseLogic&) = delete;

    virtual ~BaseLogic() = default;
    //-----------------------------------------------

    void set_ptr_to_foes_logic(BaseLogic& link_to_foes_logic);

    void send_attack_vector();

    void receive_attack_vector(const coords_vector& attack_vector);

    const coords_bool_vector& pass_processed_foes_attack_vector_to_middleman() const;

    void react_to_attack();

    size_t get_n_o_active_guns() const {
        return coords_of_active_guns.size();
    }

    void set_deployment_number(size_t n);

    size_t get_deployment_number() const {
        return deployment_number;
    }

    void virtual reset_self();

protected:
    coords_vector own_attack_vector;
    coords_vector coords_of_active_guns;

private:
    coords_bool_vector foes_attack_vector;
    BaseLogic * foes_logic = nullptr;
    size_t deployment_number;
};

#endif // BASEUNDERLYINGLOGIC_H
