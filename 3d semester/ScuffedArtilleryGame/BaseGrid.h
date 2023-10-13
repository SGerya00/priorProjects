#ifndef BASEGRID_H
#define BASEGRID_H

#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QPainter>

#include "COLOURS_DEF.h"

typedef std::vector< std::pair< std::pair<int, int>, bool > > coords_bool_vector;
typedef std::pair< std::pair<int, int>, bool > coords_bool_pair;
typedef std::pair<int, int> coords_pair;
typedef std::vector< std::pair<int, int> > coords_vector;

class BaseGrid : public QWidget
{
    Q_OBJECT
public:
    BaseGrid() = delete;

    explicit BaseGrid(QWidget *parent = nullptr);

    BaseGrid(const BaseGrid&) = delete;

    BaseGrid& operator=(const BaseGrid&) = delete;

    virtual ~BaseGrid() = default;
    //---------------------------------------------

    void repaint_grid();

    void receive_ref_to_vector_of_attack(const coords_bool_vector& vector_of_attack);

public slots:
    void disable_self();

    void enable_self();

protected:
    void virtual reset_grid();

    void resizeEvent(QResizeEvent* event) override;

    void paintEvent(QPaintEvent *event) override;

    void paint_event_additional_squares(QPainter& painter, int grid_square_size_pix);

    void paint_on_end(bool result, QPainter& painter);

    Qt::GlobalColor status_to_colour_converter(bool status) const;

    int calculate_displayed_size() const;

    coords_pair pix_coord_to_grid_coord_converter(int x_coord_pix, int y_coord_pix) const;

    int get_grid_square_size_pix() const;

    bool check_mouse_click_inside(int x, int y) const;

    const coords_bool_vector& get_additional_squares_vector() const {
        return additional_squares;
    }

    int get_height_shift() const {
        return height_shift;
    }

    void set_game_over_true();

    void set_game_over_status(bool value);

private:
    int height_shift;
    QPen pen;
    QBrush brush;
    coords_bool_vector additional_squares;
    //it holds grid coordinates (starting from 0) and colour of square
    bool game_over = false;
    bool game_over_status = false;
};

#endif // BASEGRID_H
