#ifndef YOURGRID_H
#define YOURGRID_H

#include <QWidget>

#include "BaseGrid.h"

class YourGrid : public BaseGrid
{
    Q_OBJECT

public:
    YourGrid() = delete;

    explicit YourGrid(QWidget *parent = nullptr);

    YourGrid(const YourGrid&) = delete;

    YourGrid& operator=(const YourGrid&) = delete;

    virtual ~YourGrid() = default;
    //---------------------------------------------

    void repaint_your_grid();

    void mousePressEvent(QMouseEvent *event) override;

    void set_deployment_number(size_t n);

    const coords_vector& pass_own_deployment_vector_to_middleman() const;

    void receive_ref_to_vector_of_attack(const coords_bool_vector& vector_of_attack);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void _signal_enable_START_button(); //is emmited when enough squares are pressed

private slots:
    void flush_your_deployment_vector();

    void paint_self_acc_to_result(bool, bool);

    void reset_grid() override;

private:
    size_t your_deployment_number;
    coords_vector your_deployment;
};

#endif // YOURGRID_H
