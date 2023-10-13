#ifndef MIDDLEMAN_H
#define MIDDLEMAN_H

#include <QObject>

#include "yourgrid.h"
#include "foesgrid.h"
#include "ownlogic.h"
#include "foeslogic.h"

constexpr size_t INITIAL_N_O_GUNS = 5;

class MiddleMan : public QObject
{
    Q_OBJECT

public:
    MiddleMan() = delete;

    MiddleMan(YourGrid * const your_grid, FoesGrid * const foes_grid);

    MiddleMan(const MiddleMan&) = delete;

    MiddleMan& operator=(const MiddleMan&) = delete;

    virtual ~MiddleMan() = default;
    //-----------------------------------------------------------------

    void set_the_game_up();

    //pass hitmarkers from foes_grid to own_logic to make attack vector--------------------
    void make_own_logic_attack_vector();
    //-------------------------------------------------------------------------------------

    //take processed vector of foes attack and pass it to own grid to modify picture-------
    void send_ref_to_foes_vector_of_attack_to_own_grid();
    //-------------------------------------------------------------------------------------

    //take processed vector of foes attack and pass it to foes logic to analyze------------
    void send_ref_to_processed_foes_vector_of_attack_to_foes_logic();
    //-------------------------------------------------------------------------------------

    //take processed vector of own attack and pass it to foes grid to modify picture-------
    void send_ref_to_own_vector_of_attack_to_foes_grid();
    //-------------------------------------------------------------------------------------

    //tell foes_grid how many hit markers should there be to continue----------------------
    void pass_own_logic_n_o_active_guns_to_foes_grid();
    //-------------------------------------------------------------------------------------

    //button manipulations-----------------------------------------------------------------
signals:
    void _signal_disable_Reset_deployment_button();

    void _signal_disable_START_button();

    void _signal_disable_Reset_barrage_button();

    void _signal_disable_Give_order_button();
    //-------------------------------------------------------------------------------------

    //widget manipulations-----------------------------------------------------------------
signals:
    void _signal_disable_Your_grid();

    void _signal_disable_Foes_grid();

    void _signal_paint_on_end(bool, bool);

    void _signal_game_is_played(short int);
    //-------------------------------------------------------------------------------------

public slots:
    void give_own_logic_deployment_from_your_grid();

    void play_game();
    //emits _signal_game_is_played with result
    //(1 for your victory, -1 for your defeat, 0 for draw, 2 for continue)

    void game_is_possibly_over(short int result);
    //disables all if game is over

    void restart_game();
    //restart both logics, clear painting vectors and logic vectors

private:
    const coords_vector& get_ref_to_vector_of_hit_markers_from_foes_grid() const;

    const coords_bool_vector& get_ref_to_foes_vector_of_attack_from_own_logic() const;

    const coords_bool_vector& get_ref_to_own_vector_of_attack_from_foes_logic() const;

    size_t get_own_logic_n_o_active_guns() const;

    void play_one_round();

    YourGrid * const p_your_grid;
    FoesGrid * const p_foes_grid;
    OwnLogic own_logic;
    FoesLogic foes_logic;
};

#endif // MIDDLEMAN_H
