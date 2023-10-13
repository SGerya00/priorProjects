#ifndef FOESLOGIC_H
#define FOESLOGIC_H

#include <random>

#include "BaseUnderlyingLogic.h"

class FoesLogic : public BaseLogic
{
public:
    FoesLogic();

    FoesLogic(const FoesLogic&) = delete;

    FoesLogic& operator=(const FoesLogic&) = delete;

    virtual ~FoesLogic() = default;
    //------------------------------------------------

    void make_attack_vector();

    void deploy_guns();

    void process_attack_info(const coords_bool_vector& processed_attack_vector);

    void reset_self() override;

private:
    int make_random_number();

    coords_vector already_hit;
    std::mt19937 random_device;
};

#endif // FOESLOGIC_H
