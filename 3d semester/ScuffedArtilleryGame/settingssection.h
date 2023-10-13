#ifndef SETTINGSSECTION_H
#define SETTINGSSECTION_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class SettingsSection : public QWidget
{
    Q_OBJECT
public:
    SettingsSection() = delete;

    explicit SettingsSection(QWidget *parent = nullptr);

    SettingsSection(const SettingsSection&) = delete;

    SettingsSection& operator=(const SettingsSection&) = delete;

    virtual ~SettingsSection() = default;
    //-----------------------------------------------------------

    void make_connections();

public slots:
    void disable_reset_deployment_button();

    void enable_reset_deployment_button();

    void disable_START_button();

    void enable_START_button();

    void disable_reset_barrage_button();

    void enable_reset_barrage_button();

    void disable_give_order_button();

    void enable_give_order_button();

    //cover-ups
    void reset_deployment_button_pressed();

    void START_button_pressed();

    void reset_barrage_button_pressed();

    void give_order_button_pressed();

    void restart_game_button_pressed();
    //

signals:
    void _signal_reset_deployment_button_pressed();

    void _signal_START_button_pressed();

    void _signal_reset_barrage_button_pressed();

    void _signal_give_order_button_pressed();

    void _signal_restart_game_button_pressed();

private:
    QVBoxLayout vertical_layout;

    QPushButton reset_deployment_button;
    QPushButton START_button;
    QPushButton reset_barrage_button;
    QPushButton give_order_button;
    QPushButton restart_game_button;
};

#endif // SETTINGSSECTION_H
