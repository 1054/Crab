#include "specPainter.h"
#include "math.h"

specPainter::specPainter(QWidget *parent) :
    QWidget(parent)
{
    m_o.setX(32);
    m_o.setY(32);
    m_f = this->font();
    m_fontmetr = new QFontMetrics(m_f);
}

void specPainter::paintEvent(QPaintEvent *event)
{
    QPainter pntr(this); int i=0;
    pntr.setPen(QColor(0,0,0));
    //draw box
    pntr.drawRect(m_o.x(),m_o.y(),this->width()-2*m_o.x(),this->height()-2*m_o.y());
    double dx=m_o.x();
    double dy=m_o.y();
    if(!m_BottomLines.isEmpty())
    {
        pntr.setMatrix(QMatrix(1,0,0,-1,dx,this->height()-dy)); //x = 1*y + dx, y = -1*x + h - dy
        pntr.drawLines(m_BottomLines);
        pntr.setMatrix(QMatrix(1,0,0,+1,dx,this->height()-dy));
        for(i=0; i<m_BottomMainLines.size(); i++)
        {
//            qDebug()<<"m_BottomMainLinesString[i].size()"<<m_BottomMainLinesString[i].size();
//            qDebug()<<"m_BottomMainLinesString[i].length()"<<m_BottomMainLinesString[i].length();
//            qDebug()<<"m_fontmetr->width"<<m_fontmetr->width(m_BottomMainLinesString[i]);
            //绘制横坐标的刻度数值, 注意稍微偏移一些使得刻度数值的中部与刻度线对齐.
            pntr.drawText(m_BottomMainLines[i].x() - m_fontmetr->width(m_BottomMainLinesString[i])/2,\
                          m_BottomMainLines[i].y() + m_f.pointSize() + 1,\
                          m_BottomMainLinesString[i]);
        }
    }
    if(!m_TopLines.isEmpty())
    {
        pntr.setMatrix(QMatrix(1,0,0,1,dx,dy));
        pntr.drawLines(m_TopLines);
        //pntr.setMatrix(QMatrix(1,0,0,1,dx,dy-5));
        //for(i=0; i<m_TopMainLines.size(); i++)
        //    pntr.drawText(m_TopMainLines[i],m_TopMainLinesString[i]);
    }
    if(!m_LeftLines.isEmpty())
    {
        //pntr.setMatrix(QMatrix(0,1,-1,0,this->height()-dy,-dx));
        pntr.setMatrix(QMatrix(0,-1,1,0,dx,this->height()-dy)); //x = 1*y + dx, y = -1*x + h - dy
        pntr.drawLines(m_LeftLines);
        pntr.setMatrix(QMatrix(0,-1,1,0,dx,this->height()-dy));
        for(i=0; i<m_LeftMainLines.size(); i++)
            pntr.drawText(m_LeftMainLines[i].x() - m_fontmetr->width(m_LeftMainLinesString[i])/2,\
                          m_LeftMainLines[i].y() - 2,\
                          m_LeftMainLinesString[i]);
    }
    if(!m_RightLines.isEmpty())
    {
        pntr.setMatrix(QMatrix(0,-1,-1,0,this->width()-dx,this->height()-dy));
        pntr.drawLines(m_RightLines);
    }
    if(!m_DataPoints.isEmpty())
    {
        pntr.setMatrix(QMatrix(1,0,0,-1,dx,this->height()-dy)); //x = 1*x + dx, y = -1*y + h - dy
//        pntr.drawPoints(m_DataPoints.data(), m_DataSize);
        pntr.drawPolyline(m_DataPoints.data(), m_DataSize);
    }
    //done
    pntr.setMatrix(QMatrix(1,0,0,1,0,0));
    QWidget::paintEvent(event);
}

void specPainter::resizeEvent(QResizeEvent *event)
{
    int dw = (this->width()-2*m_o.x());
    int dh = (this->height()-2*m_o.y());
    /*将物理坐标转换到像素坐标*/
    if(!m_x.isEmpty())
    {
        m_BottomLines     = m_x.Lines2Points(dw);
        m_BottomMainLines = m_x.MainLines2Points(dw);
        m_TopLines        = m_BottomLines;
    }
    if(!m_y.isEmpty())
    {
        m_LeftLines       = m_y.Lines2Points(dh);
        m_LeftMainLines   = m_y.MainLines2Points(dh);
        m_RightLines      = m_LeftLines;
    }
    if(!m_x.isEmpty() && !m_y.isEmpty())
    {
        //plot data points.
        m_DataSize = m_x.sizeofData();
        m_DataPoints.clear();
        QVector<QPointF> xPoints = m_x.Data2Points(dw);
        QVector<QPointF> yPoints = m_y.Data2Points(dh);
        for(int i=0; i<m_DataSize; i++)
            m_DataPoints.append(QPointF(xPoints[i].x(),yPoints[i].x()));
    }
    QWidget::resizeEvent(event);
}

void specPainter::setxdata(double *data, int size)
{
    //read data into m_x.
    m_x.setxdata(data,size);
    m_BottomMainLinesString.clear();
    for(int i=0; i<m_x.sizeofMainLines(); i++)
        m_BottomMainLinesString.append(m_x.getMainLinesString(i));
//    m_TopMainLinesString.clear();
//    for(int i=0; i<m_t.sizeofMainLines(); i++)
//        m_TopMainLinesString.append(m_x.getMainLinesString(i));
}

void specPainter::setydata(double *data, int size)
{
    m_y.setydata(data,size);
    m_LeftMainLinesString.clear();
    for(int i=0; i<m_y.sizeofMainLines(); i++)
        m_LeftMainLinesString.append(m_y.getMainLinesString(i));
}

void specPainter::setfont(QFont ft)
{
    this->setFont(ft);
    m_f = ft;
    delete(m_fontmetr);
    m_fontmetr = new QFontMetrics(m_f);
    return;
}
