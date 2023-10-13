#ifndef OWNLOGIC_H
#define OWNLOGIC_H

#include "BaseUnderlyingLogic.h"
//#include "foesgrid.h"

class OwnLogic : public BaseLogic
{
public:
    OwnLogic() = default;

    OwnLogic(const OwnLogic&) = delete;

    OwnLogic& operator=(const OwnLogic&) = delete;

    virtual ~OwnLogic() = default;
    //----------------------------------------------

    void make_attack_vector(const coords_vector& v_o_hit_markers);

    void receive_deployment_vector(const coords_vector& deployment_vector);
};

#endif // OWNLOGIC_H
