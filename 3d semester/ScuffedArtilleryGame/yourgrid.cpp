#include "yourgrid.h"

#include <QEvent>
#include <QMouseEvent>

#include "COLOURS_DEF.h"

YourGrid::YourGrid(QWidget *parent) : BaseGrid(parent)
{
    //nothing
}

void YourGrid::repaint_your_grid() {
    update();
}

void YourGrid::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        //check if already max number of squares clicked
        if (your_deployment.size() >= your_deployment_number) {
            return;
        }
        //check if clicked on an already existing square
        int event_x_coord_pix = (event->pos()).x();
        int event_y_coord_pix = (event->pos()).y();
        if (false == check_mouse_click_inside(event_x_coord_pix, event_y_coord_pix)) {
            return;
            //mouse was clicked inside of a widget, but outside of displayed area
        }
        coords_pair grid_coords =
                pix_coord_to_grid_coord_converter(event_x_coord_pix, event_y_coord_pix);
        for (size_t i = 0; i < your_deployment.size(); i++) {
            int a = abs(your_deployment[i].first - grid_coords.first);
            int b = abs(your_deployment[i].second - grid_coords.second);
            if ((a <= 1) && (b <= 1)) {
                return;
                //mouse was clicked on an already existing square or in it's vicinity
            }

        }
        coords_pair new_position{grid_coords};
        your_deployment.push_back(new_position);
        if (your_deployment.size() == your_deployment_number) {
            emit _signal_enable_START_button();
        }
        repaint_your_grid();
    } else {
        BaseGrid::mousePressEvent(event);
    }
}

void YourGrid::set_deployment_number(size_t n) {
    your_deployment_number = n;
}

const coords_vector& YourGrid::pass_own_deployment_vector_to_middleman() const {
    return your_deployment;
}

void YourGrid::receive_ref_to_vector_of_attack(const coords_bool_vector& vector_of_attack) {
    BaseGrid::receive_ref_to_vector_of_attack(vector_of_attack);

    for (size_t i = 0; i < vector_of_attack.size(); i++) {
        for (size_t j = 0; j < your_deployment.size(); j++) {
            if (your_deployment[j] == vector_of_attack[i].first) {
                your_deployment.erase(your_deployment.begin() + j);
            }
        }
    }
}

//YourGrid::~YourGrid() {} is default

void YourGrid::paintEvent(QPaintEvent *event) {
    //repaint grid
    BaseGrid::paintEvent(event);
    //paint your existing deployment
    QPen temp_pen;

    QBrush temp_brush;
    temp_brush.setColor(ARTCOLOUR);
    temp_brush.setStyle(Qt::SolidPattern);

    QPainter painter(this);
    painter.save();

    QPoint point;
    point.setX(0);
    point.setY(get_height_shift());
    painter.translate(point);

    painter.setPen(temp_pen);
    painter.setBrush(temp_brush);
    for (size_t i = 0; i < your_deployment.size(); i++) {
        auto u = (your_deployment)[i];
        int x_coord = u.first * BaseGrid::get_grid_square_size_pix();
        int y_coord = u.second * BaseGrid::get_grid_square_size_pix();
        int side = get_grid_square_size_pix();
        QRect square(x_coord, y_coord, side, side);
        painter.drawRect(square);
    }
    painter.restore();
}

//slots
void YourGrid::flush_your_deployment_vector() {
    your_deployment.clear();
    repaint_your_grid();
}

void YourGrid::paint_self_acc_to_result(bool r1, bool r2) {
    set_game_over_true();
    set_game_over_status(r1);
    BaseGrid::update();
}

void YourGrid::reset_grid() {
    BaseGrid::reset_grid();
    flush_your_deployment_vector();
}
