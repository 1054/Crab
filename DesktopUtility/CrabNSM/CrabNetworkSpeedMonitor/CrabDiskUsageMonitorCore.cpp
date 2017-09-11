#include "CrabDiskUsageMonitorCore.h"

CrabDiskUsageMonitorCore::CrabDiskUsageMonitorCore(QObject *parent) : QObject(parent)
{
    // see -- http://doc.qt.io/qt-5/qstorageinfo.html

    m_disk_usage_data = QStorageInfo::root();

    //
    // convert size units
    m_disk_total_size = double(m_disk_usage_data.bytesTotal())/Kilo;
    m_disk_avail_size = double(m_disk_usage_data.bytesAvailable())/Kilo;
    m_disk_total_unit = "KB";
    m_disk_avail_unit = "KB";
    m_disk_total_prec = 1;
    m_disk_avail_prec = 1;
    if(m_disk_total_size>=Kilo) {
        m_disk_total_size = m_disk_total_size/Kilo;
        m_disk_total_unit = "MB";
        m_disk_total_prec = 2;
    }
    if(m_disk_total_size>=Kilo) {
        m_disk_total_size = m_disk_total_size/Kilo;
        m_disk_total_unit = "GB";
        m_disk_total_prec = 3;
    }
    if(m_disk_avail_size>=Kilo) {
        m_disk_avail_size = m_disk_avail_size/Kilo;
        m_disk_avail_unit = "MB";
        m_disk_avail_prec = 2;
    }
    if(m_disk_avail_size>=Kilo) {
        m_disk_avail_size = m_disk_avail_size/Kilo;
        m_disk_avail_unit = "GB";
        m_disk_avail_prec = 3;
    }

    QStorageInfo storage = m_disk_usage_data;
    qDebug() << "path:" << storage.rootPath();
    qDebug() << "name:" << storage.name();
    qDebug() << "fileSystem:" << storage.fileSystemType();
    qDebug() << "isReadOnly:" << storage.isReadOnly();
    qDebug().noquote() << QString("totalSize: %1 %2").arg(m_disk_total_size,0,'f',m_disk_total_prec,QChar(' ')).arg(m_disk_total_unit);
    qDebug().noquote() << QString("availableSize: %1 %2").arg(m_disk_avail_size,0,'f',m_disk_avail_prec,QChar(' ')).arg(m_disk_avail_unit);

}
