#ifndef SPECVIEWER_H
#define SPECVIEWER_H

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QRect>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtGui/QFontDialog>
#include <QtGui/QFileDialog>

namespace Ui {
    class specViewer;
}

class specViewer : public QWidget
{
    Q_OBJECT

public:
    explicit specViewer(QWidget *parent = 0);
    ~specViewer();
    void setFont();
    void setFont(const QFont &);
    void setTopTitle(QString);
    void setBottomTitle(QString);
    void setLeftTitle(QString);
    void setRightTitle(QString);
    void setTopSubtt(QString);
    void setBottomSubtt(QString);
    void setLeftSubtt(QString);
    void setRightSubtt(QString);
public slots:
    void openFile();
    void openFITSFile(QString strFilePath);
    void openASCIIFile(QString strFilePath);
protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
private:
    Ui::specViewer *ui;
    int     errorcode;
    //file
    QString   m_FilePath;
    QFileInfo m_FileInfo;
    bool      m_FileOpened;
    //axes
    QFont   m_Font;
    QFont   m_FontTT;
    QPoint  m_TopPoint;
    QPoint  m_TopSubpt;
    QString m_TopTitle;
    QString m_TopSubtt;
    int     m_TopAlign;
    int     m_TopSubAl;
    QPoint  m_BottomPoint;
    QPoint  m_BottomSubpt;
    QString m_BottomTitle;
    QString m_BottomSubtt;
    int     m_BottomAlign;
    int     m_BottomSubAl;
    QPoint  m_LeftPoint;
    QPoint  m_LeftSubpt;
    int     m_LeftAlign;
    int     m_LeftSubAl;
    QString m_LeftTitle;
    QString m_LeftSubtt;
    QPoint  m_RightPoint;
    QPoint  m_RightSubpt;
    QString m_RightTitle;
    QString m_RightSubtt;
    int     m_RightAlign;
    int     m_RightSubAl;
};

#endif // SPECVIEWER_H
