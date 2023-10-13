#include "BaseGrid.h"

#include "SIZES_NMSPC.h"

BaseGrid::BaseGrid(QWidget *parent) : QWidget(parent) {
    brush.setColor(GRIDCOLOUR);
    brush.setStyle(Qt::SolidPattern);

    this->setMinimumSize(SIZES::GRID_MIN_SIDE_SIZE_PIX, SIZES::GRID_MIN_SIDE_SIZE_PIX);
}

void BaseGrid::repaint_grid() {
    update();
}

void BaseGrid::receive_ref_to_vector_of_attack(const coords_bool_vector& vector_of_attack) {
    for (size_t i = 0; i < vector_of_attack.size(); i++) {
        bool dublicate_flag = false;
        for (size_t j = 0; j < additional_squares.size(); j++) {
            if (additional_squares[j].first == (vector_of_attack)[i].first) {
                dublicate_flag = true;
                break;
            }
        }
        if (true == dublicate_flag) {
            continue;
        } else {
            additional_squares.push_back(vector_of_attack[i]);
        }
    }
    repaint_grid();
}

//BaseGrid::~BaseGrid() {} is default

//slots
void BaseGrid::disable_self() {
    this->setDisabled(true);
}

void BaseGrid::enable_self() {
    this->setEnabled(true);
}

//protected

void BaseGrid::reset_grid() {
    additional_squares.clear();
    game_over = false;
    game_over_status = false;
}

void BaseGrid::resizeEvent(QResizeEvent *event) {
    int displayed_size = calculate_displayed_size();
    int total_height = height();

    height_shift = (total_height - displayed_size) / 2;
}

void BaseGrid::paintEvent(QPaintEvent *event) {

    int displayed_side_size = calculate_displayed_size();

    QRect whole_grid(0, 0, displayed_side_size, displayed_side_size);

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);

    painter.save();

    QPoint point;
    point.setX(0);
    point.setY(height_shift);
    painter.translate(point);

    painter.drawRect(whole_grid); //draw entire field

    int grid_square_size_pix = displayed_side_size / SIZES::GRID_SIZE_IN_SQUARES;

    for (int x = 0; x < displayed_side_size; x += grid_square_size_pix) {
        painter.drawLine(x, 0, x, displayed_side_size);
    } //draw vertical
    for (int y = 0; y < displayed_side_size; y += grid_square_size_pix) {
        painter.drawLine(0, y, displayed_side_size, y);
    } //draw horizontal
    painter.drawLine(0, displayed_side_size - 1, displayed_side_size - 1, displayed_side_size - 1);
    painter.drawLine(displayed_side_size - 1, 0, displayed_side_size - 1, displayed_side_size - 1);

    //draw additional squares
    paint_event_additional_squares(painter, grid_square_size_pix);

    if (true == game_over) {
        paint_on_end(game_over_status, painter);
    }
    painter.restore();
}

void BaseGrid::paint_event_additional_squares(QPainter& painter, int grid_square_size_pix) {

    QBrush new_brush;
    new_brush.setStyle(Qt::SolidPattern);

    for (size_t i = 0; i < additional_squares.size(); i++) {
        coords_bool_pair u = (additional_squares)[i];
        int x_coord_pix = u.first.first * grid_square_size_pix;
        int y_coord_pix = u.first.second * grid_square_size_pix;
        bool square_status = u.second;
        new_brush.setColor(status_to_colour_converter(square_status));
        painter.setBrush(new_brush);
        QRect specific_square(x_coord_pix, y_coord_pix, grid_square_size_pix, grid_square_size_pix);
        //pen is the same, brush is of a different colour

        painter.drawRect(specific_square);
    }
}

void BaseGrid::paint_on_end(bool result, QPainter& painter) {

    int displayed_side_size = calculate_displayed_size();

    QRect whole_grid(0, 0, displayed_side_size, displayed_side_size);
    QBrush temp_brush;
    temp_brush.setStyle(Qt::SolidPattern);
    if (true == result) {
        temp_brush.setColor(ONENDGREEN);
    } else {
        temp_brush.setColor(ONENDRED);
    }
    painter.setBrush(temp_brush);

    painter.drawRect(whole_grid);
}

Qt::GlobalColor BaseGrid::status_to_colour_converter(bool status) const {
    if (status) {
        return HITCOLOUR;
    } else {
        return MISSCOLOUR;
    }
}

int BaseGrid::calculate_displayed_size() const {
    int width = this->width();
    int height = this->height();

    int displayed_side_size = 0;
    if (width > height) {
        displayed_side_size = (height / SIZES::GRID_SIZE_IN_SQUARES) * SIZES::GRID_SIZE_IN_SQUARES;
    } else {
        displayed_side_size = (width / SIZES::GRID_SIZE_IN_SQUARES) * SIZES::GRID_SIZE_IN_SQUARES;
    }
    return displayed_side_size;
}

bool BaseGrid::check_mouse_click_inside(int x, int y) const {
    int displayed_side_size = calculate_displayed_size();
    y -= height_shift;
    if ((x >= displayed_side_size) || (y >= displayed_side_size) || (0 > y)) {
        return false;
    }
    return true;
}

coords_pair BaseGrid::pix_coord_to_grid_coord_converter(int x_coord_pix, int y_coord_pix) const {
    int grid_square_size_pix = calculate_displayed_size() / SIZES::GRID_SIZE_IN_SQUARES;
    y_coord_pix -= height_shift;
    int x_coord_grid = x_coord_pix / grid_square_size_pix;
    int y_coord_grid = y_coord_pix / grid_square_size_pix;
    coords_pair grid_coords{x_coord_grid, y_coord_grid};
    return grid_coords;
}

int BaseGrid::get_grid_square_size_pix() const {
    int grid_square_size_pix = calculate_displayed_size() / SIZES::GRID_SIZE_IN_SQUARES;
    return grid_square_size_pix;
}

void BaseGrid::set_game_over_true() {
    game_over = true;
}

void BaseGrid::set_game_over_status(bool value) {
    game_over_status = value;
}
