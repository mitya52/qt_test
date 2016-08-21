#ifndef WINDOW_H
#define WINDOW_H

#include <helper.h>

#include <QWidget>

class Window : public QWidget
{
    Q_OBJECT

private:
    Helper helper;

public:
    Window(size_t size);
};

#endif // WINDOW_H
