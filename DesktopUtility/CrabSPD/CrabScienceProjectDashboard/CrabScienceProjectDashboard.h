#ifndef CRABSCIENCEPROJECTDASHBOARD_H
#define CRABSCIENCEPROJECTDASHBOARD_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include "CrabScienceProjectClass.h"

namespace Ui {
class CrabScienceProjectDashboard;
}

class CrabScienceProjectDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit CrabScienceProjectDashboard(QWidget *parent = nullptr);
    ~CrabScienceProjectDashboard();

    void test_task_tree();

private:
    Ui::CrabScienceProjectDashboard *ui;
    CrabScienceProjectClass* Project;
};

#endif // CRABSCIENCEPROJECTDASHBOARD_H
