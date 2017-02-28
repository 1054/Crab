#ifndef CRABRIBBONBASEUI_H
#define CRABRIBBONBASEUI_H

#include <QDebug>
#include <QFileInfo>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QWidget>

namespace Ui {
class CrabRibbonBaseUI;
}

class CrabRibbonBaseUI : public QWidget
{
    Q_OBJECT

public:
    explicit CrabRibbonBaseUI(QWidget *parent = 0);
    ~CrabRibbonBaseUI();
    void setupHead(QString label = QString());
    void setupCore(QWidget *widget = NULL);
signals:
    void signal_open_file(const QString &FilePath);
protected:
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::CrabRibbonBaseUI *ui;
    QWidget *m_CoreWidget;
    QPoint  m_MovePos;
};

#endif // CRABRIBBONBASEUI_H
