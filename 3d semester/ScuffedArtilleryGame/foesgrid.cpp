#include "foesgrid.h"

#include <QEvent>
#include <QMouseEvent>

#include "COLOURS_DEF.h"

FoesGrid::FoesGrid(QWidget *parent) : BaseGrid(parent)
{
    //nothing
}

void FoesGrid::repaint_foes_grid() {
    update();
}

const coords_vector& FoesGrid::get_ref_to_hit_markers() const {
    return hit_markers;
}

void FoesGrid::set_max_hit_markers(size_t n) {
    max_hit_markers = n;
}

void FoesGrid::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int event_x_coord_pix = (event->pos()).x();
        int event_y_coord_pix = (event->pos()).y();
        if (false == check_mouse_click_inside(event_x_coord_pix, event_y_coord_pix)) {
            return;
            //mouse was clicked inside of a widget, but outside of displayed area
        }
        coords_pair grid_coords =
                pix_coord_to_grid_coord_converter(event_x_coord_pix, event_y_coord_pix);
        coords_bool_vector already_painted = get_additional_squares_vector();
        for (size_t i = 0; i < already_painted.size(); i++) {
            if (grid_coords == already_painted[i].first) {
                return;
                //mouse was clicked on an already painted square
            }
        }
        for (size_t i = 0; i < hit_markers.size(); i++) {
            if (grid_coords == (hit_markers)[i]) {
                hit_markers.erase(hit_markers.begin() + i);
                repaint_foes_grid();
                if (hit_markers.size() + 1 == max_hit_markers) {
                    emit _signal_disable_Give_order_button();
                }
                return;
                //mouse was clicked on an already selected square
            }
        }
        if (hit_markers.size() == max_hit_markers) {
            return;
        }
        hit_markers.push_back(grid_coords);
        if (hit_markers.size() == max_hit_markers) {
            emit _signal_enable_Give_order_button();
        }
        repaint_foes_grid();
    } else {
        BaseGrid::mousePressEvent(event);
    }
}

void FoesGrid::paintEvent(QPaintEvent *event) {
    //repaint grid
    BaseGrid::paintEvent(event);
    //paint your existing deployment
    QPen temp_pen;
    temp_pen.setColor(HIT_MARKER_COLOUR);
    temp_pen.setCapStyle(Qt::SquareCap);
    temp_pen.setWidth(2);

    QBrush temp_brush;

    QPainter painter(this);
    painter.save();

    QPoint point;
    point.setX(0);
    point.setY(get_height_shift());
    painter.translate(point);

    painter.setPen(temp_pen);
    painter.setBrush(temp_brush);
    for (size_t i = 0; i < hit_markers.size(); i++) {
        auto u = (hit_markers)[i];
        int x_coord = u.first * BaseGrid::get_grid_square_size_pix();
        int y_coord = u.second * BaseGrid::get_grid_square_size_pix();
        int side = get_grid_square_size_pix();
        QRect square(x_coord, y_coord, side, side);
        QLine line1(x_coord, y_coord, x_coord + side, y_coord + side);
        QLine line2(x_coord + side, y_coord, x_coord, y_coord +side);
        painter.drawRect(square);
        painter.drawLine(line1);
        painter.drawLine(line2);
    }
    painter.restore();
}

//slots
void FoesGrid::flush_hit_markers_vector() {
    hit_markers.clear();
    repaint_foes_grid();
}

void FoesGrid::paint_self_acc_to_result(bool r1, bool r2) {
    set_game_over_true();
    set_game_over_status(r2);
    BaseGrid::update();
}

void FoesGrid::reset_grid() {
    BaseGrid::reset_grid();
    flush_hit_markers_vector();
}
