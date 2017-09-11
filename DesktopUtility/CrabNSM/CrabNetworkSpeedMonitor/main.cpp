#include "CrabNetworkSpeedMonitor.h"
#include <QThread>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CrabNetworkSpeedMonitor w;
    w.show();

    QThread::msleep(3500);
    w.update_network_speed_display();

    QThread::msleep(3500);
    w.update_network_speed_display();

    QThread::msleep(3500);
    w.update_network_speed_display();

    return a.exec();
}
