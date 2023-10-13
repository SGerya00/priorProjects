#ifndef FOESGRID_H
#define FOESGRID_H

#include <QWidget>

#include "BaseGrid.h"
#include "BaseUnderlyingLogic.h"

class FoesGrid : public BaseGrid
{
    Q_OBJECT

public:
    FoesGrid() = delete;

    explicit FoesGrid(QWidget *parent = nullptr);

    FoesGrid(const FoesGrid&) = delete;

    FoesGrid& operator=(const FoesGrid&) = delete;

    virtual ~FoesGrid() = default;
    //---------------------------------------------

    void mousePressEvent(QMouseEvent *event) override;

    void repaint_foes_grid();

    const coords_vector& get_ref_to_hit_markers() const;

    void set_max_hit_markers(size_t n);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    size_t max_hit_markers;
    coords_vector hit_markers;

signals:
    void _signal_enable_Give_order_button(); //is emmited when enough squares are pressed

    void _signal_disable_Give_order_button(); //is emmited when a square was removed by clicking on it

private slots:
    void flush_hit_markers_vector();

    void paint_self_acc_to_result(bool, bool);

    void reset_grid() override ;
};

#endif // FOESGRID_H
