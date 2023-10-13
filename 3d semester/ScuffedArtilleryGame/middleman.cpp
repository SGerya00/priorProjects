#include "middleman.h"

MiddleMan::MiddleMan(YourGrid * const your_grid, FoesGrid * const foes_grid) :
p_your_grid(your_grid), p_foes_grid(foes_grid) {
    //nothing
}

void MiddleMan::set_the_game_up() {
    own_logic.set_ptr_to_foes_logic(foes_logic);
    foes_logic.set_ptr_to_foes_logic(own_logic);
    own_logic.set_deployment_number(INITIAL_N_O_GUNS);
    foes_logic.set_deployment_number(INITIAL_N_O_GUNS);
    foes_logic.deploy_guns();
    p_foes_grid->set_max_hit_markers(INITIAL_N_O_GUNS);
    p_your_grid->set_deployment_number(INITIAL_N_O_GUNS);

    emit _signal_disable_START_button();
    emit _signal_disable_Reset_barrage_button();
    emit _signal_disable_Give_order_button();
    emit _signal_disable_Foes_grid();
}

//pass hitmarkers from foes_grid to own_logic to make attack vector--------------------
const coords_vector& MiddleMan::get_ref_to_vector_of_hit_markers_from_foes_grid() const {
    return p_foes_grid->get_ref_to_hit_markers();
}

void MiddleMan::make_own_logic_attack_vector() {
    const coords_vector& ref = get_ref_to_vector_of_hit_markers_from_foes_grid();
    own_logic.make_attack_vector(ref);
}
//-------------------------------------------------------------------------------------

//take processed vector of foes attack and pass it to own grid to modify picture-------
const coords_bool_vector& MiddleMan::get_ref_to_foes_vector_of_attack_from_own_logic() const {
    return own_logic.pass_processed_foes_attack_vector_to_middleman();
}

void MiddleMan::send_ref_to_foes_vector_of_attack_to_own_grid() {
    const coords_bool_vector& ref = get_ref_to_foes_vector_of_attack_from_own_logic();
    p_your_grid->receive_ref_to_vector_of_attack(ref);
}
//-------------------------------------------------------------------------------------

//take processed vector of foes attack and pass it to foes logic to analyze------------
//const coords_bool_vector& MiddleMan::get_ref_to_foes_vector_of_attack_from_own_logic() {}

void MiddleMan::send_ref_to_processed_foes_vector_of_attack_to_foes_logic() {
    const coords_bool_vector& ref = get_ref_to_foes_vector_of_attack_from_own_logic();
    foes_logic.process_attack_info(ref);
}
//-------------------------------------------------------------------------------------

//take processed vector of own attack and pass it to foes grid to modify picture-------
const coords_bool_vector& MiddleMan::get_ref_to_own_vector_of_attack_from_foes_logic() const {
    return foes_logic.pass_processed_foes_attack_vector_to_middleman();
}

void MiddleMan::send_ref_to_own_vector_of_attack_to_foes_grid() {
    const coords_bool_vector& ref = get_ref_to_own_vector_of_attack_from_foes_logic();
    p_foes_grid->receive_ref_to_vector_of_attack(ref);
}
//-------------------------------------------------------------------------------------

//tell foes_grid how many hit markers should there be to continue----------------------
size_t MiddleMan::get_own_logic_n_o_active_guns() const {
    return own_logic.get_n_o_active_guns();
}

void MiddleMan::pass_own_logic_n_o_active_guns_to_foes_grid() {
    size_t n = get_own_logic_n_o_active_guns();
    p_foes_grid->set_max_hit_markers(n);
}
//-------------------------------------------------------------------------------------

//MiddleMan::~MiddleMan() {} is default

//slots
void MiddleMan::give_own_logic_deployment_from_your_grid() {
    const coords_vector& ref = p_your_grid->pass_own_deployment_vector_to_middleman();
    own_logic.receive_deployment_vector(ref);
}

void MiddleMan::play_game() {
    play_one_round();
    short int result = 2;
    size_t n_o_own_guns = own_logic.get_n_o_active_guns();
    size_t n_o_foes_guns = foes_logic.get_n_o_active_guns();
    if ((0 == n_o_own_guns) && (0 != n_o_foes_guns)) {
        result = -1;
    } else if ((0 == n_o_foes_guns) && (0 != n_o_own_guns)) {
        result = 1;
    } else if ((0 == n_o_foes_guns) && (0 == n_o_own_guns)) {
        result = 0;
    }
    emit _signal_game_is_played(result);
}
//1 for your victory, -1 for your defeat, 0 for draw, 2 for continue
//emits _signal_game_is_played with result

void MiddleMan::game_is_possibly_over(short int result) {
    if (result != 2) {
        emit _signal_disable_Reset_deployment_button();
        emit _signal_disable_START_button();
        emit _signal_disable_Reset_barrage_button();
        emit _signal_disable_Give_order_button();
        emit _signal_disable_Your_grid();
        emit _signal_disable_Foes_grid();
        if (1 == result) {
            //Congrats!
            emit _signal_paint_on_end(1, 0);
        } else if (-1 == result) {
            //Boohoo you lost :(
            emit _signal_paint_on_end(0, 1);
        } else {
            //it's a... draw...
            emit _signal_paint_on_end(0, 0);
        }
    } else {
        //nothing
    }
}
//disables all if game is over

void MiddleMan::restart_game() {
    own_logic.reset_self();
    foes_logic.reset_self();
    set_the_game_up();
}

void MiddleMan::play_one_round() {
    //STAGE 1
    //make own attack vector (for that, get hit markers)
    make_own_logic_attack_vector();

    //launch attack on foe
    own_logic.send_attack_vector();

    //pass processed vector of own attack to foes grid for it to modify painting
    send_ref_to_own_vector_of_attack_to_foes_grid();

    //STAGE 2
    //make foes attack vector
    foes_logic.make_attack_vector();

    //launch foes attack on self
    foes_logic.send_attack_vector();

    //pass processed vector of foes attack to own grid for it to modify painting
    send_ref_to_foes_vector_of_attack_to_own_grid();

    //pass processed vector of foes attack back to foe for it to analyze
    send_ref_to_processed_foes_vector_of_attack_to_foes_logic();

    //STAGE 3
    //tell foes grid how many guns are left
    pass_own_logic_n_o_active_guns_to_foes_grid();

    //END OF ROUND
}

