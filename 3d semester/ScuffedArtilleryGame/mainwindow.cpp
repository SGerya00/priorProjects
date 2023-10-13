#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SIZES_NMSPC.h"

MainWindow::MainWindow() :
    horizontal_layout(this), your_grid_widget(this), foes_grid_widget(this), settings_section_widget(this)
{   
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    horizontal_layout.addWidget(&your_grid_widget);
    horizontal_layout.addWidget(&foes_grid_widget);
    horizontal_layout.addWidget(&settings_section_widget);

    settings_section_widget.make_connections();

    this->setMinimumSize(SIZES::MAIN_WINDOW_MIN_WIDTH_PIX, SIZES::MAIN_WINDOW_MIN_HEIGHT_PIX);
}

//MainWindow::~MainWindow() {} is default
