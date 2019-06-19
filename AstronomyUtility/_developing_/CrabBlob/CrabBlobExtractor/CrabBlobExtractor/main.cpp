#include "CrabBlobExtractor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CrabBlobExtractor w;
    w.show();

    return a.exec();
}
