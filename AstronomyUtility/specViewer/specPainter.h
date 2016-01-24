#ifndef SPECPAINTER_H
#define SPECPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QMatrix>
#include <QFontMetrics>
#include "specAxis.h"

class specPainter : public QWidget
{
    Q_OBJECT
public:
    explicit specPainter(QWidget *parent = 0);
    void setxdata(double *data, int size);
    void setydata(double *data, int size);
    void setfont(QFont);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    specAxis         m_x;
    specAxis         m_y;
    QPoint           m_o;
    QFont            m_f;
    QFontMetrics    *m_fontmetr;
    int              m_DataSize;
    QVector<QPointF> m_DataPoints;
    QVector<QPointF> m_TopLines;
    QVector<QPointF> m_TopMainLines;
    QVector<QString> m_TopMainLinesString;
    QVector<QPointF> m_BottomLines;
    QVector<QPointF> m_BottomMainLines;
    QVector<QString> m_BottomMainLinesString;
    QVector<QPointF> m_LeftLines;
    QVector<QPointF> m_LeftMainLines;
    QVector<QString> m_LeftMainLinesString;
    QVector<QPointF> m_RightLines;

};

#endif // SPECPAINTER_H
