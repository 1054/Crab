#include "CrabScienceProjectDashboard.h"
#include "ui_CrabScienceProjectDashboard.h"

CrabScienceProjectDashboard::CrabScienceProjectDashboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrabScienceProjectDashboard)
{
    ui->setupUi(this);

    Project = new CrabScienceProjectClass();

    //openCrabScienceProjectByPath();

    test_task_tree();


}


CrabScienceProjectDashboard::~CrabScienceProjectDashboard()
{
    delete ui;
    if(Project) {delete Project;}
}


void CrabScienceProjectDashboard::test_task_tree()
{
    // test
    Project->test_task_tree();
    ui->treeView_TaskTab->setModel(Project->TaskTreeModel);

}
