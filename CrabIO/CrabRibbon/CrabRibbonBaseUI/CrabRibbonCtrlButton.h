#ifndef CRABRIBBONCTRLBUTTON_H
#define CRABRIBBONCTRLBUTTON_H

#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>

class CrabRibbonCtrlButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CrabRibbonCtrlButton(QWidget *parent = 0);
    void   setupMenu(QMenu *menu);
    void   setupColors(QColor BackgroundColor, QColor IntermediateColor, QColor TextColor);

signals:
    void bye();

public slots:
    void doEveryTimeUnderHover();
    void doWhenMenuTriggered(QAction* act);
    void doWhenMenuShow();
    void doWhenMenuClose();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void   setupTimer();
    void   showMenu(); // override
    void   closeMenu();
    QTimer *m_Timer;
    int     m_Hover;
    QMenu  *m_Menu;
    bool    m_MenuOpened;
    QColor  m_BgColor;
    QColor  m_CgColor;
    QColor  m_DgColor;

};

#endif // CRABRIBBONCTRLBUTTON_H
