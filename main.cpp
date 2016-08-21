#include "window.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    size_t size = (argc == 2 ? std::atoi(argv[1]) : 3);

    Window window(size);
    window.show();

    return app.exec();
}
