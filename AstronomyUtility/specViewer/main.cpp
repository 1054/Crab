#include <QtGui/QApplication>
#include "specViewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    specViewer w;
    w.show();

    return a.exec();
}
