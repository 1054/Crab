#ifndef CRABPHOTUI_H
#define CRABPHOTUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class CrabPhotUI
{

public:
    explicit CrabPhotUI();
    ~CrabPhotUI();
    
    void show();
    
private:
    QVBoxLayout *m_Layout0;
    QPushButton *m_ButtonOK;
    QWidget *m_Ui;
};

#endif // CRABPHOTUI_H
