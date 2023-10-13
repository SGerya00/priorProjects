#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
//----------------------------------------
#include <QSplitter>
//----------------------------------------

#include "yourgrid.h"
#include "foesgrid.h"
#include "settingssection.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();

    MainWindow(const MainWindow&) = delete;

    MainWindow& operator=(const MainWindow&) = delete;

    virtual ~MainWindow() = default;
    //--------------------------------------------------

    YourGrid * get_ptr_to_your_grid() {
        return &your_grid_widget;
    }

    FoesGrid * get_ptr_to_foes_grid() {
        return &foes_grid_widget;
    }

    SettingsSection * get_ptr_to_settings_section() {
        return &settings_section_widget;
    }

private:
    QHBoxLayout horizontal_layout;

    YourGrid your_grid_widget;
    FoesGrid foes_grid_widget;
    SettingsSection settings_section_widget;
};
#endif // MAINWINDOW_H
