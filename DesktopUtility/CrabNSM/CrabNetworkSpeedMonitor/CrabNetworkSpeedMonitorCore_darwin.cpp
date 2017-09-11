#include "CrabNetworkSpeedMonitorCore_darwin.h"

CrabNetworkSpeedMonitorCore::CrabNetworkSpeedMonitorCore(QObject *parent)
{
    m_total_down_Bytes = u_int64_t(0);
    m_total_up_Bytes = u_int64_t(0);
    m_time_in_ms = u_int64_t(0);
    Q_UNUSED(parent);
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(print_network_speed()));
}


void CrabNetworkSpeedMonitorCore::update_network_speed_data(double update_interval_in_seconds)
{
    long update_interval_in_ms_long = long(update_interval_in_seconds*1e3);
    int update_interval_in_ms = 0;
    if(update_interval_in_ms_long>long(INT_MAX)) {
        update_interval_in_ms = INT_MAX;
    } else {
        update_interval_in_ms = int(update_interval_in_ms_long);
    }
    if(update_interval_in_ms>0) {
        if(!m_timer->isActive()) {
            m_timer->start(update_interval_in_ms);
        } else {
            m_timer->setInterval(update_interval_in_ms);
        }
    } else {
        m_timer->stop();
    }
}


QStringList CrabNetworkSpeedMonitorCore::get_network_speed_data()
{
    return m_network_speed_data;
}


QStringList CrabNetworkSpeedMonitorCore::print_os_info()
{
    // see -- https://stackoverflow.com/questions/11072804/how-do-i-determine-the-os-version-at-runtime-in-os-x-or-ios-without-using-gesta
    //
    // get kern.osrelease by sysctlbyname()
    char str[256];
    size_t size = sizeof(str);
    int ret = sysctlbyname("kern.osrelease", str, &size, NULL, 0);
    Q_UNUSED(ret);
    qDebug() << "Darwin Kernel Version:" << str;
    //
    // convert Darwin Kernel Version to Mac OS Version
    QString os_version_str = QString(str);
    QStringList os_version_code = os_version_str.split(".");
    QStringList os_version_info;
    if(os_version_code.at(0).toInt()>=16) {
        os_version_info.append("macOS");
    } else {
        os_version_info.append("Mac OS X");
    }
    os_version_info.append(QString("%1").arg(10,0,'d',0,' '));
    os_version_info.append(QString("%1").arg(os_version_code.at(0).toInt()-4,0,'d',0,' '));
    os_version_info.append(QString("%1").arg(os_version_code.at(1).toInt()-1,0,'d',0,' '));
    switch (os_version_code.at(0).toInt()) {
        case 16: os_version_info << "Sierra"; break;
        case 15: os_version_info << "El Capitan"; break;
        case 14: os_version_info << "Yosemite"; break;
        case 13: os_version_info << "Mavericks"; break;
        case 12: os_version_info << "Mountain Lion"; break;
        case 11: os_version_info << "Lion"; break;
        case 10: os_version_info << "Snow Leopard"; break;
        case  9: os_version_info << "Leopard"; break;
        case  8: os_version_info << "Tiger"; break;
        case  7: os_version_info << "Panther"; break;
        case  6: os_version_info << "Jaguar"; break;
        case  5: os_version_info << "Puma"; break;
        default: break;
    }
    qDebug() << "Mac OS Version:" <<
                QString("%1 %2.%3.%4 %5").arg(os_version_info.at(0)).arg(os_version_info.at(1)).arg(os_version_info.at(2)).arg(os_version_info.at(3)).arg(os_version_info.at(4));
    return os_version_info;
}


QStringList CrabNetworkSpeedMonitorCore::print_network_speed()
{
    // see -- https://stackoverflow.com/questions/1126790/how-to-get-network-adapter-stats-in-linux-mac-osx/1385692
    int mib[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST2, 0 };
    size_t len;
    int ret = sysctl(mib, 6, NULL, &len, NULL, 0);
    if(ret<0) {fprintf(stderr, "sysctl: %s\n", strerror(errno)); exit(1);}
    //
    // call sysctl
    char *buf = (char *)malloc(len);
    ret = sysctl(mib, 6, buf, &len, NULL, 0);
    if(ret<0) {fprintf(stderr, "sysctl: %s\n", strerror(errno)); exit(1);}
    //
    // call sysctl and read totalibytes and totalobytes
    char *lim = buf + len;
    char *next = NULL;
    u_int64_t totalibytes = 0;
    u_int64_t totalobytes = 0;
    for(next = buf; next < lim; ) {
        struct if_msghdr *ifm = (struct if_msghdr *)next;
        next += ifm->ifm_msglen;
        if (ifm->ifm_type == RTM_IFINFO2) {
            struct if_msghdr2 *if2m = (struct if_msghdr2 *)ifm;
            totalibytes += if2m->ifm_data.ifi_ibytes;
            totalobytes += if2m->ifm_data.ifi_obytes;
        }
    }
    //
    // get current time and print debug info
    m_previous_time_in_ms = m_time_in_ms;
    m_time_in_ms = QDateTime::currentMSecsSinceEpoch();
    //
    // store previous measures
    m_previous_total_down_Bytes = m_total_down_Bytes;
    m_total_down_Bytes = totalibytes;
    m_previous_total_up_Bytes = m_total_up_Bytes;
    m_total_up_Bytes = totalobytes;
    //
    // convert to double values
    double total_down_KB = double(totalibytes)/Kilo; // KB
    double total_up_KB = double(totalobytes)/Kilo; // KB
    double speed_down_KB = -99.0;
    double speed_up_KB = -99.0;
    //
    // check previous_network_speed_data, compute speed
    //qDebug() << "m_previous_network_speed_data =" << m_previous_network_speed_data;
    //qDebug() << "m_network_speed_data =" << m_network_speed_data;
    QStringList previous_network_speed_data = m_previous_network_speed_data;
    if(m_previous_time_in_ms>0) {
        qDebug().noquote() << QString("buffer length %1, total ibytes %2, total obytes %3, current time in ms %4, past time in ms %5.").arg(len).arg(totalibytes).arg(totalobytes).arg(m_time_in_ms).arg(m_time_in_ms-m_previous_time_in_ms);
        speed_down_KB = double(m_total_down_Bytes - m_previous_total_down_Bytes) / double(m_time_in_ms - m_previous_time_in_ms); // KB/s
        speed_up_KB = double(m_total_up_Bytes - m_previous_total_up_Bytes) / double(m_time_in_ms - m_previous_time_in_ms); // KB/s
    } else {
        qDebug().noquote() << QString("buffer length %1, total ibytes %2, total obytes %3, current time in ms %4.").arg(len).arg(totalibytes).arg(totalobytes).arg(m_time_in_ms);
        /* -- note that qDebug().noquote() is Qt 5.4 feature,
         * see -- https://stackoverflow.com/questions/27976581/why-is-qstring-printed-with-quotation-marks
         * see -- https://stackoverflow.com/questions/39560530/how-to-print-qstring-containing-line-breaks-using-qdebug
         */
    }
    //
    // determine display unit
    QString total_down_unit = "KB";
    QString total_up_unit = "KB";
    QString speed_down_unit = "KB/s";
    QString speed_up_unit = "KB/s";
    int total_down_prec = 1;
    int total_up_prec = 1;
    int speed_down_prec = 1;
    int speed_up_prec = 1;
    if(total_down_KB>=Kilo) {
        total_down_KB = total_down_KB/Kilo; total_down_unit = "MB";
    }
    if(total_up_KB>=Kilo) {
        total_up_KB = total_up_KB/Kilo; total_up_unit = "MB";
    }
    if(speed_down_KB>=Kilo) {
        speed_down_KB = speed_down_KB/Kilo; speed_down_unit = "MB/s";
    }
    if(speed_up_KB>=Kilo) {
        speed_up_KB = speed_up_KB/Kilo; speed_up_unit = "MB/s";
    }
    if(total_down_KB>=Kilo) {
        total_down_KB = total_down_KB/Kilo; total_down_unit = "GB"; total_down_prec = 2;
    }
    if(total_up_KB>=Kilo) {
        total_up_KB = total_up_KB/Kilo; total_up_unit = "GB"; total_up_prec = 2;
    }
    if(speed_down_KB>=Kilo) {
        speed_down_KB = speed_down_KB/Kilo; speed_down_unit = "GB/s"; speed_down_prec = 2;
    }
    if(speed_up_KB>=Kilo) {
        speed_up_KB = speed_up_KB/Kilo; speed_up_unit = "GB/s"; speed_up_prec = 2;
    }
    //
    // return
    QStringList network_speed_data;
    if(speed_down_KB>=0.0) {
        network_speed_data.append(QString("DOWN %1 %2").arg(speed_down_KB,0,'f',speed_down_prec,0).arg(speed_down_unit));
    } else {
        network_speed_data.append(QString("DOWN N/A"));
    }
    if(speed_up_KB>=0.0) {
        network_speed_data.append(QString("UP   %1 %2").arg(speed_up_KB,0,'f',speed_up_prec,0).arg(speed_up_unit));
    } else {
        network_speed_data.append(QString("UP   N/A"));
    }
    if(total_down_KB>=0.0) {
        network_speed_data.append(QString("TOTAL DOWN %1 %2").arg(total_down_KB,0,'f',total_down_prec,0).arg(total_down_unit)); // input
    } else {
        network_speed_data.append(QString("TOTAL DOWN N/A"));
    }
    if(total_up_KB>=0.0) {
        network_speed_data.append(QString("TOTAL UP   %1 %2").arg(total_up_KB,0,'f',total_up_prec,0).arg(total_up_unit)); // output
    } else {
        network_speed_data.append(QString("TOTAL UP   N/A"));
    }
    network_speed_data.append(QString("%1").arg(m_time_in_ms)); // Current Time in milliseconds
    m_previous_network_speed_data = m_network_speed_data;
    m_network_speed_data = network_speed_data;
    //qDebug() << "m_previous_network_speed_data =" << m_previous_network_speed_data;
    //qDebug() << "m_network_speed_data =" << m_network_speed_data;
    emit updated_network_speed_data();
    return network_speed_data;
}





