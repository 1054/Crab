#include "CrabRibbonCtrlButton.h"

CrabRibbonCtrlButton::CrabRibbonCtrlButton(QWidget *parent) :
    QPushButton(parent)
{
    /*
     * make button text shadow ?
     */
    /*
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect( );
    effect->setOffset(0.5,-0.5);
    effect->setBlurRadius(1);
    effect->setColor(Qt::black);
    this->setGraphicsEffect(effect);
    */

    /*
     * timer
     */
    m_Timer = NULL;
    setupTimer();

    /*
     * menu
     */
    m_Menu = NULL; m_MenuOpened = false;
    // setupMenu(); // do this externally

    /*
     * colors
     */
    setupColors(QColor("#67a9cf"),QColor("#3690c0"),Qt::white);

}



void CrabRibbonCtrlButton::setupColors(QColor BackgroundColor, QColor IntermediateColor, QColor TextColor)
{
    m_BgColor = BackgroundColor;
    m_CgColor = IntermediateColor;
    m_DgColor = TextColor;
}



void CrabRibbonCtrlButton::doEveryTimeUnderHover()
{
    /*
     * when this button is under hover, we graduately change the background,
     * then finally open the menu
     */
    qreal TotalHoverTime = (qreal)m_Hover*400.0/1000.0/4.0; // times interval then divided by total seconds.
    // qDebug() << "CrabRibbonCtrlButton::doEveryTimeUnderHover()" << m_Hover << TotalHoverTime;
    if(TotalHoverTime>=0.0 && TotalHoverTime<1.0) {
        this->setStyleSheet(QString(":hover{background-color:qlineargradient(x1:0, y1:%1, x2:0, y2:1, stop:0 %2, stop:0.5 %3, stop:1 %4);}").arg(1.0-TotalHoverTime).arg(m_BgColor.name()).arg(m_CgColor.name()).arg(m_BgColor.name()));
        m_Hover++;
    } else if (TotalHoverTime>=1.0) {
        m_Timer->stop();
        // qDebug() << "CrabRibbonCtrlButton::doEveryTimeUnderHover()" << "this->showMenu()";
        this->showMenu();
    }
}



void CrabRibbonCtrlButton::doWhenMenuTriggered(QAction* act)
{
    if(act->text().toUpper() == QString("BYE")) {
        emit bye();
    }
}



void CrabRibbonCtrlButton::doWhenMenuShow()
{
    // qDebug() << "CrabRibbonCtrlButton::doWhenMenuShow()";
    if(m_Timer) { m_Timer->stop(); m_Hover=-1; this->setStyleSheet(QString("CrabRibbonCtrlButton:hover{background-color:qlineargradient(x1:0, y1:%1, x2:0, y2:1, stop:0 %2, stop:0.5 %3, stop:1 %4);}").arg(0).arg(m_BgColor.name()).arg(m_CgColor.name()).arg(m_BgColor.name())); }
    // qDebug() << this->styleSheet();
    // this->showMenu();
    m_MenuOpened = true;
}



void CrabRibbonCtrlButton::doWhenMenuClose()
{
    // qDebug() << "CrabRibbonCtrlButton::doWhenMenuClose()";
    if(m_Timer) { m_Timer->stop(); m_Hover=-1; this->setStyleSheet(""); }
    // this->closeMenu();
    m_MenuOpened = false;
}



void CrabRibbonCtrlButton::enterEvent(QEvent *event)
{
    /*
     * start timer when under hover (and menu is not open)
     */
    // qDebug() << "CrabRibbonCtrlButton::enterEvent()";
    if(m_Menu && !m_MenuOpened) { if(m_Timer) { m_Timer->start(); m_Hover=0; } }
    QPushButton::enterEvent(event);
}



void CrabRibbonCtrlButton::leaveEvent(QEvent *event)
{
    /*
     * stop timer when leave hover (and menu is not open)
     */
    // qDebug() << "CrabRibbonCtrlButton::leaveEvent()";
    if(!m_MenuOpened) { if(m_Timer) { m_Timer->stop(); m_Hover=-1; this->setStyleSheet(""); } }
    QPushButton::leaveEvent(event);
}


void CrabRibbonCtrlButton::mousePressEvent(QMouseEvent *event)
{
    /*
     * stop timer and show menu when mouse left-button pressed
     */
    if(Qt::LeftButton==event->button()) {
        // qDebug() << "CrabRibbonCtrlButton::mousePressEvent()" << "left button pressed!" << m_MenuOpened;
        if(m_MenuOpened) {this->closeMenu();} else {this->showMenu();}
    }
    QWidget::mousePressEvent(event); // QPushButton::mousePressEvent(event);
}



void CrabRibbonCtrlButton::showMenu()
{
    /*
     * see qpushbutton.cpp#L568
       Q_D(QPushButton); if(!d||!d->menu) return;
       setDown(true); d->_q_popupPressed();
     */
    if(m_Menu) {
        QPoint pos = this->mapToGlobal(this->rect().topLeft());
        pos.setY(pos.y() - m_Menu->sizeHint().height());
        // qDebug() << "CrabRibbonCtrlButton::showMenu()";
        // m_Menu->setNoReplayFor(this);
        m_MenuOpened = true;
        m_Menu->exec(pos);
    }
}



void CrabRibbonCtrlButton::closeMenu()
{
    if(m_Menu) {
        m_Menu->close();
        m_MenuOpened=false;
    }
}




void CrabRibbonCtrlButton::setupTimer()
{
    /*
     * timer
     */
    m_Timer = new QTimer(this); m_Timer->setInterval(400); m_Hover=-1;
    connect(m_Timer,SIGNAL(timeout()),this,SLOT(doEveryTimeUnderHover()));
}


void CrabRibbonCtrlButton::setupMenu(QMenu *menu)
{
    /*
     * menu
     */
    if(m_Menu) { delete m_Menu; m_Menu=NULL; m_MenuOpened=false; }
    if(menu) { m_Menu=menu; m_MenuOpened=false;
        m_Menu->setStyleSheet(QString("QMenu{background-color:%1;color:%2;}QMenu::item:selected{background-color:%3;}").arg(m_BgColor.name()).arg(m_DgColor.name()).arg(m_CgColor.name()));
        this->setMenu(m_Menu); connect(m_Menu,SIGNAL(triggered(QAction*)),this,SLOT(doWhenMenuTriggered(QAction*)));
        connect(m_Menu,SIGNAL(aboutToShow()),this,SLOT(doWhenMenuShow())); connect(m_Menu,SIGNAL(aboutToHide()),this,SLOT(doWhenMenuClose()));
    }
}
