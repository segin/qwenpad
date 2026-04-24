#include <QApplication>
#include "qwenpad.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Qwenpad w;
    w.show();
    return app.exec();
}
