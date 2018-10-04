#include "CrabScienceProjectDashboard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CrabScienceProjectDashboard w;
    w.show();

    return a.exec();
}
