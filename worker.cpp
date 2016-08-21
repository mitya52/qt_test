#include "worker.h"

#include <QMutexLocker>

#include <atomic>
#include <cmath>

Worker::Vector2D::Vector2D(const QVector2D &other):
    x(other.x()), y(other.y())
{}

Worker::Vector2D& Worker::Vector2D::operator+=(const QVector2D &other)
{
    x += other.x();
    y += other.y();

    return *this;
}

QVector2D Worker::Vector2D::get() const
{
    return QVector2D(x, y);
}

Worker::Worker(GLWidget *widget):
    widget(widget)
{}

void Worker::correctWithForce(QVector2D &v)
{
    double inverseR = 1. / std::max(v.length(), 0.2F);
    v.normalize();
    v *= (inverseR * (1 - inverseR));
}

void Worker::unsafeCompute()
{
    size_t size = newForce.size();
    QVector2D forceIToJ;

    for(size_t i = 0; i < size; i++)
    {
        for(size_t j = i + 1; j < size; j++)
        {
            forceIToJ = widget->current[j].pos - widget->current[i].pos;
            correctWithForce(forceIToJ);

            newForce[i] += forceIToJ;
            newForce[j] -= forceIToJ;
        }
    }
}

void Worker::unsafeComputeParallel(size_t n_jobs)
{
    size_t size = newForce.size();

    std::vector<std::thread> threads(n_jobs);
    std::vector<std::atomic<Vector2D>> forces(size);

    for(size_t i = 0; i < size; i++)
        forces[i].store({newForce[i]});

    auto fun = [this, &forces, n_jobs, size](unsigned n)
    {
        QVector2D forceIToJ;

        for(size_t i = n; i < size; i += n_jobs)
        {
            for(size_t j = i + 1; j < size; j++)
            {
                forceIToJ = this->widget->current[j].pos -
                            this->widget->current[i].pos;
                correctWithForce(forceIToJ);

                auto adder = [&forces](int ind, QVector2D forceIToJ)
                {
                    auto oldVal = forces[ind].load();
                    decltype(oldVal) newVal;
                    do {
                        newVal = oldVal;
                        newVal += forceIToJ;
                    } while(!forces[ind].compare_exchange_weak(oldVal, newVal));
                };

                adder(i, forceIToJ);
                adder(j, -forceIToJ);
            }
        }
    };

    for(size_t i = 0; i < n_jobs; i++)
        threads[i] = std::thread(fun, i);

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    for(size_t i = 0; i < size; i++)
        newForce[i] = forces[i].load().get();
}

void Worker::process()
{
    widget->lock.lockForRead();

    size_t size = widget->current.size();

    newForce.clear();
    newForce.reserve(size);
    std::transform(widget->current.begin(), widget->current.end(),
                   std::back_inserter(newForce), [](const GLWidget::point_t &p)
                   { return p.force; });

    size_t n_jobs = std::thread::hardware_concurrency() - 1;
    size < 1000 || n_jobs < 2 ? unsafeCompute() : unsafeComputeParallel(n_jobs);

    widget->lock.unlock();

    widget->lock.lockForWrite();

    widget->next.clear();
    widget->next.reserve(size);

    for(size_t i = 0; i < size; i++)
    {
        auto newPoint = widget->current[i].pos + newForce[i];

        widget->next.push_back(GLWidget::point_t(newPoint, newForce[i]));
    }

    widget->lock.unlock();
}
