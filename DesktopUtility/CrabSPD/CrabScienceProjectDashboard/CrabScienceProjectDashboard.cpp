#include "CrabScienceProjectDashboard.h"
#include "ui_CrabScienceProjectDashboard.h"

CrabScienceProjectDashboard::CrabScienceProjectDashboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrabScienceProjectDashboard)
{
    ui->setupUi(this);
}

CrabScienceProjectDashboard::~CrabScienceProjectDashboard()
{
    delete ui;
}
