#ifndef CRABSCIENCEPROJECTDASHBOARD_H
#define CRABSCIENCEPROJECTDASHBOARD_H

#include <QWidget>

namespace Ui {
class CrabScienceProjectDashboard;
}

class CrabScienceProjectDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit CrabScienceProjectDashboard(QWidget *parent = 0);
    ~CrabScienceProjectDashboard();

private:
    Ui::CrabScienceProjectDashboard *ui;
};

#endif // CRABSCIENCEPROJECTDASHBOARD_H
