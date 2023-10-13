#include <QApplication>
#include <QPushButton>

#include "mainwindow.h"
#include "middleman.h"
#include "yourgrid.h"
#include "foesgrid.h"
#include "settingssection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    MiddleMan middleman(w.get_ptr_to_your_grid(), w.get_ptr_to_foes_grid());

    YourGrid *your_grid = w.get_ptr_to_your_grid();
    FoesGrid *foes_grid = w.get_ptr_to_foes_grid();
    QObject::connect(&middleman, SIGNAL(_signal_disable_Your_grid()), your_grid, SLOT(disable_self()));
    QObject::connect(&middleman, SIGNAL(_signal_disable_Foes_grid()), foes_grid, SLOT(disable_self()));

    SettingsSection * settings_section = w.get_ptr_to_settings_section();
    QObject::connect(&middleman, SIGNAL(_signal_disable_Reset_deployment_button()),
                     settings_section, SLOT(disable_reset_deployment_button()));
    QObject::connect(&middleman, SIGNAL(_signal_disable_START_button()),
                     settings_section, SLOT(disable_START_button()));
    QObject::connect(&middleman, SIGNAL(_signal_disable_Reset_barrage_button()),
                     settings_section, SLOT(disable_reset_barrage_button()));
    QObject::connect(&middleman, SIGNAL(_signal_disable_Give_order_button()),
                     settings_section, SLOT(disable_give_order_button()));

    QObject::connect(foes_grid, SIGNAL(_signal_enable_Give_order_button()),
                     settings_section, SLOT(enable_give_order_button()));
    QObject::connect(foes_grid, SIGNAL(_signal_disable_Give_order_button()),
                     settings_section, SLOT(disable_give_order_button()));
    QObject::connect(your_grid, SIGNAL(_signal_enable_START_button()),
                     settings_section, SLOT(enable_START_button()));

    QObject::connect(settings_section, SIGNAL(_signal_reset_deployment_button_pressed()),
                     your_grid, SLOT(flush_your_deployment_vector()));
    QObject::connect(settings_section, SIGNAL(_signal_reset_deployment_button_pressed()),
                     settings_section, SLOT(disable_START_button()));
    QObject::connect(settings_section, SIGNAL(_signal_reset_barrage_button_pressed()),
                     foes_grid, SLOT(flush_hit_markers_vector()));
    QObject::connect(settings_section, SIGNAL(_signal_reset_barrage_button_pressed()),
                     settings_section, SLOT(disable_give_order_button()));

    QObject::connect(settings_section, SIGNAL(_signal_START_button_pressed()),
                     your_grid, SLOT(disable_self()));
    QObject::connect(settings_section, SIGNAL(_signal_START_button_pressed()),
                     &middleman, SLOT(give_own_logic_deployment_from_your_grid()));
    QObject::connect(settings_section, SIGNAL(_signal_START_button_pressed()),
                     foes_grid, SLOT(enable_self()));
    QObject::connect(settings_section, SIGNAL(_signal_START_button_pressed()),
                     settings_section, SLOT(disable_reset_deployment_button()));
    QObject::connect(settings_section, SIGNAL(_signal_START_button_pressed()),
                     settings_section, SLOT(disable_START_button()));
    QObject::connect(settings_section, SIGNAL(_signal_START_button_pressed()),
                     settings_section, SLOT(enable_reset_barrage_button()));

    QObject::connect(settings_section, SIGNAL(_signal_give_order_button_pressed()),
                     &middleman, SLOT(play_game()));
    QObject::connect(settings_section, SIGNAL(_signal_give_order_button_pressed()),
                     foes_grid, SLOT(flush_hit_markers_vector()));
    QObject::connect(settings_section, SIGNAL(_signal_give_order_button_pressed()),
                     settings_section, SLOT(disable_give_order_button()));
    QObject::connect(&middleman, SIGNAL(_signal_game_is_played(short int)),
                     &middleman, SLOT(game_is_possibly_over(short int)));

    //for end-game widget painting
    QObject::connect(&middleman, SIGNAL(_signal_paint_on_end(bool,bool)),
                     your_grid, SLOT(paint_self_acc_to_result(bool,bool)));
    QObject::connect(&middleman, SIGNAL(_signal_paint_on_end(bool,bool)),
                     foes_grid, SLOT(paint_self_acc_to_result(bool,bool)));

    //for restarting the game
    QObject::connect(settings_section, SIGNAL(_signal_restart_game_button_pressed()),
                     your_grid, SLOT(reset_grid()));
    QObject::connect(settings_section, SIGNAL(_signal_restart_game_button_pressed()),
                     your_grid, SLOT(enable_self()));
    QObject::connect(settings_section, SIGNAL(_signal_restart_game_button_pressed()),
                     settings_section, SLOT(enable_reset_deployment_button()));
    QObject::connect(settings_section, SIGNAL(_signal_restart_game_button_pressed()),
                     foes_grid, SLOT(reset_grid()));
    QObject::connect(settings_section, SIGNAL(_signal_restart_game_button_pressed()),
                     &middleman, SLOT(restart_game()));

    middleman.set_the_game_up();
    w.show();
    return a.exec();
}
