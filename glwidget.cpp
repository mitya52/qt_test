#include "glwidget.h"

#include "helper.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPoint>

#include <algorithm>
#include <ctime>
#include <random>

GLWidget::GLWidget(QWidget *parent, Helper *helper, size_t size,
                   QSize fieldSize, unsigned radius):
    QOpenGLWidget(parent), helper(helper), fieldSize(fieldSize),
    radius(radius), isCaptured(false)
{
    std::mt19937 mt(std::random_device{}());
    std::uniform_real_distribution<float> uidWidth(0, fieldSize.width()),
                                          uidHeight(0, fieldSize.height());
    next.reserve(size);

    for(size_t i = 0; i < size; i++)
    {
        point_t newPoint({uidWidth(mt), uidHeight(mt)});

        next.push_back(newPoint);
    }

    setFixedSize(fieldSize + QSize(radius, radius));
    setAutoFillBackground(false);
    setMouseTracking(true);
}

void GLWidget::onTimeout()
{
    lock.lockForWrite();

    size_t size = next.size();

    current.clear();
    current.reserve(size + added.size());

    for(size_t i = 0; i < next.size(); i++)
    {
        if(removed.find(i) != removed.end())
            continue;

        current.push_back(next[i]);
    }

    removed.clear();

    current.insert(current.end(), added.begin(), added.end());
    added.clear();

    lock.unlock();

    emit startWorker();
    update();
}

void GLWidget::mousePressProcess(QMouseEvent *event)
{
    lock.lockForWrite();

    size_t size = current.size();
    size_t removeIndex = size;
    QVector2D pos(event->pos());

    for(size_t i = 0; i < size; i++)
    {
        QVector2D h = pos - current[i].pos;
        uint dist = std::hypot(h.x(), h.y());

        if(dist <= radius)
        {
            removeIndex = i;
            break;
        }
    }

    if(removeIndex != size)
    {
        removed.insert(removeIndex);

        if(event->button() == Qt::LeftButton)
        {
            isCaptured = true;
            capturedPoint = pos;
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        added.push_back(point_t(pos));
    }

    lock.unlock();
}

void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    std::vector<QPoint> points;

    lock.lockForRead();

    points.reserve(current.size() + 1);
    std::transform(current.begin(), current.end(), std::back_inserter(points),
                   [](const point_t &p) { return QPoint(p.pos.x(), p.pos.y()); });

    if(isCaptured)
        points.push_back(QPoint(capturedPoint.x(), capturedPoint.y()));

    lock.unlock();

    this->helper->paint(&painter, event, points, radius);
    painter.end();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    switch(event->button())
    {
    case Qt::RightButton:
    case Qt::LeftButton:
        mousePressProcess(event);
        break;
    default:
        break;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    lock.lockForWrite();

    if(isCaptured)
        capturedPoint = QVector2D(event->pos());

    lock.unlock();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    lock.lockForWrite();

    if(isCaptured)
    {
        added.push_back(capturedPoint);
        isCaptured = false;
    }

    lock.unlock();
}
