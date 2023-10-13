#include "settingssection.h"

#include "SIZES_NMSPC.h"

SettingsSection::SettingsSection(QWidget *parent) :
    QWidget(parent),
    vertical_layout(this),
    reset_deployment_button("&Reset deployment", this),
    START_button("&START", this),
    reset_barrage_button("&Reset barrage", this),
    give_order_button("&Give order", this),
    restart_game_button("&Restart game", this)
{

    reset_deployment_button.setFocusPolicy(Qt::NoFocus);
    START_button.setFocusPolicy(Qt::NoFocus);
    reset_barrage_button.setFocusPolicy(Qt::NoFocus);
    give_order_button.setFocusPolicy(Qt::NoFocus);
    restart_game_button.setFocusPolicy(Qt::NoFocus);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    QPalette pal = restart_game_button.palette();
    pal.setColor(QPalette::Button, Qt::red);
    restart_game_button.setPalette(pal);
    restart_game_button.setAutoFillBackground(true);

    give_order_button.setShortcut(Qt::Key_Space);

    vertical_layout.addWidget(&reset_deployment_button);
    vertical_layout.addWidget(&START_button);
    vertical_layout.addWidget(&reset_barrage_button);
    vertical_layout.addWidget(&give_order_button);
    vertical_layout.addWidget(&restart_game_button);

    this->setFixedSize(SIZES::SETTINGS_SECTION_WIDTH_PIX, SIZES::SETTINGS_SECTION_HEIGHT_PIX);
}

void SettingsSection::make_connections() {

    QPushButton * rdb = &(this->reset_deployment_button);
    QObject::connect(rdb, SIGNAL(pressed()), this, SLOT(reset_deployment_button_pressed()));
    QPushButton * START = &(this->START_button);
    QObject::connect(START, SIGNAL(pressed()), this, SLOT(START_button_pressed()));
    QPushButton * rbb = &(this->reset_barrage_button);
    QObject::connect(rbb, SIGNAL(pressed()), this, SLOT(reset_barrage_button_pressed()));
    QPushButton * gob = &(this->give_order_button);
    QObject::connect(gob, SIGNAL(pressed()), this, SLOT(give_order_button_pressed()));
    QPushButton * rgb = &(this->restart_game_button);
    QObject::connect(rgb, SIGNAL(pressed()), this, SLOT(restart_game_button_pressed()));

}

//SettingsSection::~SettingsSection() {} is default

//slots
void SettingsSection::disable_reset_deployment_button() {
    reset_deployment_button.setDisabled(true);
}

void SettingsSection::enable_reset_deployment_button() {
    reset_deployment_button.setEnabled(true);
}

void SettingsSection::disable_START_button() {
    START_button.setDisabled(true);
}

void SettingsSection::enable_START_button() {
    START_button.setEnabled(true);
}

void SettingsSection::disable_reset_barrage_button() {
    reset_barrage_button.setDisabled(true);
}

void SettingsSection::enable_reset_barrage_button() {
    reset_barrage_button.setEnabled(true);
}

void SettingsSection::disable_give_order_button() {
    give_order_button.setDisabled(true);
}

void SettingsSection::enable_give_order_button() {
    give_order_button.setEnabled(true);
}

//cover-ups
void SettingsSection::reset_deployment_button_pressed() {
    emit _signal_reset_deployment_button_pressed();
}

void SettingsSection::START_button_pressed() {
    emit _signal_START_button_pressed();
}

void SettingsSection::reset_barrage_button_pressed() {
    emit _signal_reset_barrage_button_pressed();
}

void SettingsSection::give_order_button_pressed() {
    emit _signal_give_order_button_pressed();
}

void SettingsSection::restart_game_button_pressed() {
    emit _signal_restart_game_button_pressed();
}
//
