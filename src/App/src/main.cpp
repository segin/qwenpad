#include <QApplication>
#include "qwenpad.h"
#include <QUrl>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList files;
    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        QFileInfo fi(arg);
        if (fi.exists()) {
            files.append(arg);
        }
    }

    Qwenpad window;
    if (!files.isEmpty()) {
        window.openFiles(files);
    }
    window.show();
    return app.exec();
}
