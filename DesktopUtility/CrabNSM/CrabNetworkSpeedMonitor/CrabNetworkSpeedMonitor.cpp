#include "CrabNetworkSpeedMonitor.h"
#include "ui_CrabNetworkSpeedMonitor.h"

CrabNetworkSpeedMonitor::CrabNetworkSpeedMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrabNetworkSpeedMonitor)
{
    //
    // setup UI
    ui->setupUi(this);
    //
    // initialize m_network_speed_monitor_core
    m_network_speed_monitor_core = new CrabNetworkSpeedMonitorCore();
    //
    // print os info
    QStringList t_os_info = m_network_speed_monitor_core->print_os_info();
    if(t_os_info.size()>=5) {
        ui->label_os->setText(
                    QString("%1 %2.%3.%4 %5").arg(t_os_info.at(0)).arg(t_os_info.at(1)).arg(t_os_info.at(2)).arg(t_os_info.at(3)).arg(t_os_info.at(4))
        );
    }
    //
    // print network speed (initialize)
    //this->update_network_speed_display();
    //
    // connect ui->doubleSpinBox
    connect(ui->doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(update_network_speed_display_interval(double)));
    ui->doubleSpinBox->setValue(1.50);
    ui->doubleSpinBox->setSingleStep(0.25);
    //
    // connect updated_network_speed_data
    connect(m_network_speed_monitor_core,SIGNAL(updated_network_speed_data()),this,SLOT(update_network_speed_display()));
    //
    // get disk usage info
    m_disk_usage_monitor_core = new CrabDiskUsageMonitorCore();

}


CrabNetworkSpeedMonitor::~CrabNetworkSpeedMonitor()
{
    delete ui;
}


void CrabNetworkSpeedMonitor::update_network_speed_display_interval(double update_interval_in_seconds)
{
    if(m_network_speed_monitor_core) {
        //
        // update_network_speed_data
        m_network_speed_monitor_core->update_network_speed_data(update_interval_in_seconds);
    }
}


void CrabNetworkSpeedMonitor::update_network_speed_display()
{
    if(m_network_speed_monitor_core) {
        //
        // get_network_speed_data
        QStringList t_Network_Speed_Data = m_network_speed_monitor_core->get_network_speed_data();
        if(t_Network_Speed_Data.size()>=2) {
            ui->lineEdit_1->setText(t_Network_Speed_Data.at(0));
            ui->lineEdit_2->setText(t_Network_Speed_Data.at(1));
        }
        if(t_Network_Speed_Data.size()>=4) {
            ui->lineEdit_3->setText(t_Network_Speed_Data.at(2));
            ui->lineEdit_4->setText(t_Network_Speed_Data.at(3));
        }
        if(t_Network_Speed_Data.size()>=5) {
            ui->label_ms->setText(t_Network_Speed_Data.at(4));
        }
    }
}
