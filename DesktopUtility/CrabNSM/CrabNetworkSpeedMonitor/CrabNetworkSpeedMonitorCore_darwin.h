#ifndef CRABNETWORKSPEEDMONITORCORE_DARWIN_H
#define CRABNETWORKSPEEDMONITORCORE_DARWIN_H

//#import <Foundation/Foundation.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/sysctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/route.h>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QDebug>

#define Kilo 1024.0


class CrabNetworkSpeedMonitorCore : public QObject
{
    Q_OBJECT

public:
    explicit CrabNetworkSpeedMonitorCore(QObject *parent = NULL);
    void update_network_speed_data(double update_interval_in_seconds = 0.0);
    QStringList get_network_speed_data();

signals:
    void updated_network_speed_data();

public slots:
    QStringList print_os_info(); // output is a string list, first item is os system name, second and later items are version codes.
    QStringList print_network_speed();

private:
    QStringList m_network_speed_data;
    QStringList m_previous_network_speed_data;
    u_int64_t m_total_down_Bytes;
    u_int64_t m_total_up_Bytes;
    u_int64_t m_time_in_ms;
    u_int64_t m_previous_total_down_Bytes;
    u_int64_t m_previous_total_up_Bytes;
    u_int64_t m_previous_time_in_ms;
    QTimer *m_timer;

};

#endif // CRABNETWORKSPEEDMONITORCORE_DARWIN_H
