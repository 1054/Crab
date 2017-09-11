#ifndef CRABDISKUSAGEMONITORCORE_H
#define CRABDISKUSAGEMONITORCORE_H

#include <QObject>
#include <QStorageInfo>
#include <QDebug>

#define Kilo 1000.0


class CrabDiskUsageMonitorCore : public QObject
{
    Q_OBJECT

public:
    explicit CrabDiskUsageMonitorCore(QObject *parent = nullptr);

signals:

public slots:

private:
    QStorageInfo m_disk_usage_data;
    double m_disk_avail_size;
    double m_disk_total_size;
    QString m_disk_avail_unit;
    QString m_disk_total_unit;
    int m_disk_avail_prec;
    int m_disk_total_prec;

};

#endif // CRABDISKUSAGEMONITORCORE_H
