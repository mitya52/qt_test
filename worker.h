#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QVector2D>

#include <glwidget.h>

#include <thread>

class GLWidget;

class Worker : public QObject
{
    Q_OBJECT

    class Vector2D
    {
        float x;
        float y;

    public:
        Vector2D() noexcept = default;
        Vector2D(const QVector2D &other);
        Vector2D& operator+=(const QVector2D &other);
        QVector2D get() const;
    };

    GLWidget *widget;

    std::vector<QVector2D> newForce;

private:
    void correctWithForce(QVector2D &v);
    void unsafeCompute();
    void unsafeComputeParallel(size_t n_jobs);

public:
    Worker(GLWidget *widget);

public slots:
    void process();
};

#endif // WORKER_H
