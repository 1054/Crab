#include "CrabRibbonBaseUI.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CrabRibbonBaseUI w;
    w.show();

    return a.exec();
}
