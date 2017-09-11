#ifndef CRABNETWORKSPEEDMONITOR_H
#define CRABNETWORKSPEEDMONITOR_H


#include <QNetworkInterface> /* http://doc.qt.io/qt-5/qnetworkinterface.html */
#include <QThread>
#include <QWidget>

#ifdef Q_OS_MAC
#include "CrabNetworkSpeedMonitorCore_darwin.h"
#endif

#include "CrabDiskUsageMonitorCore.h"

namespace Ui {
class CrabNetworkSpeedMonitor;
}


class CrabNetworkSpeedMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit CrabNetworkSpeedMonitor(QWidget *parent = 0);
    ~CrabNetworkSpeedMonitor();

public slots:
    void update_network_speed_display_interval(double update_interval_in_seconds = 1.5);
    void update_network_speed_display();

private:
    Ui::CrabNetworkSpeedMonitor *ui;
    CrabNetworkSpeedMonitorCore *m_network_speed_monitor_core;
    CrabDiskUsageMonitorCore *m_disk_usage_monitor_core;

};

#endif // CRABNETWORKSPEEDMONITOR_H
