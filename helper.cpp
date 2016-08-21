#include "helper.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWidget>

#include <algorithm>

Helper::Helper():
    background(QBrush(Qt::white)),
    circleBrush(QBrush(Qt::black)),
    circlePen(QPen(Qt::green))
{}

void Helper::paint(QPainter *painter, QPaintEvent *event,
                   const points_t &points, unsigned radius)
{
    painter->fillRect(event->rect(), this->background);

    painter->save();

    painter->setBrush(this->circleBrush);
    painter->setPen(this->circlePen);

    std::for_each(points.begin(), points.end(), [&](const QPoint &point)
        { painter->drawEllipse(QRectF(point.x() - radius, point.y() - radius,
                                      radius << 1, radius << 1)); });

    painter->restore();
}
