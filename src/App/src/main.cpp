#include <QApplication>
#include "qwenpad.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Qwenpad window;
    window.show();
    return app.exec();
}
