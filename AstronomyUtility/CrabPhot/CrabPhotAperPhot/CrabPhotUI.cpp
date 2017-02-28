#include "CrabPhotUI.h"

CrabPhotUI::CrabPhotUI()
{
    m_Ui = new QWidget();
    m_Layout0 = new QVBoxLayout();
    m_Ui->setLayout(m_Layout0);
    // add a spacer
    m_Layout0->addSpacing(m_Ui->sizeHint().height()*0.75);
    // add an OK button
    m_ButtonOK = new QPushButton("OK");
    m_Layout0->addWidget(m_ButtonOK);
    m_Ui->show();
    connect
}

CrabPhotUI::~CrabPhotUI()
{
    delete m_Ui;
}

void CrabPhotUI::ok()
{
    m_Ui->show();
}
