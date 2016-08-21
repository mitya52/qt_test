#ifndef HELPER_H
#define HELPER_H

#include <QBrush>
#include <QPen>
#include <QPoint>
#include <QWidget>

#include <vector>

class Helper
{
    using points_t = std::vector<QPoint>;

public:
    Helper();

    void paint(QPainter *painter, QPaintEvent *event,
               const points_t &points, unsigned radius);

private:
    QBrush background;
    QBrush circleBrush;
    QPen circlePen;
};

#endif // HELPER_H
