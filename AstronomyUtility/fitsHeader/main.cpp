#include <QApplication>
#include "fitsHeader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fitsHeader w;
    w.show();

    return a.exec();
}
