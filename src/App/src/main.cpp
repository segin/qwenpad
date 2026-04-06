#include <QApplication>
#include "notepadapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NotepadApp window;
    window.show();
    return app.exec();
}
