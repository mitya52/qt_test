#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "worker.h"

#include <QOpenGLWidget>
#include <QReadWriteLock>
#include <QSize>
#include <QVector2D>

#include <vector>
#include <unordered_set>

class Helper;

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

    friend class Worker;

    struct Point
    {
        QVector2D pos;
        QVector2D force;

        Point(const QVector2D &p = QVector2D(0, 0),
              const QVector2D &f = QVector2D(0, 0)):
            pos(p), force(f)
        {}
    };

private:
    using point_t = Point;
    using points_t = std::vector<point_t>;
    using removed_t = std::unordered_set<size_t>;

    QReadWriteLock lock;

    Helper *helper;

    points_t current;
    points_t next;

    points_t added;
    removed_t removed;

    QSize fieldSize;
    unsigned radius;

    bool isCaptured;
    QVector2D capturedPoint;

private:
    void mousePressProcess(QMouseEvent *event);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public:
    GLWidget(QWidget *parent, Helper *helper, size_t size = 10,
             QSize fieldSize = QSize(800, 600), unsigned radius = 10);

public slots:
    void onTimeout();

signals:
    void startWorker();
};

#endif // GLWIDGET_H
