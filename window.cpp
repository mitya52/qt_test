#include "window.h"

#include "glwidget.h"
#include "worker.h"

#include <QGridLayout>
#include <QLabel>
#include <QThread>
#include <QTimer>

Window::Window(size_t size)
{
    setWindowTitle(tr("Bubbles"));

    GLWidget *glwidget = new GLWidget(this, &helper, size);
    QLabel *gllabel = new QLabel(tr("OpenGL"));
    gllabel->setAlignment(Qt::AlignCenter);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(glwidget, 0, 0);
    layout->addWidget(gllabel, 1, 0);
    setLayout(layout);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            glwidget, &GLWidget::onTimeout);

    Worker *worker = new Worker(glwidget);
    connect(glwidget, &GLWidget::startWorker,
            worker, &Worker::process);

    QThread *workerThread = new QThread;
    worker->moveToThread(workerThread);

    timer->start(30);
    workerThread->start();
}
