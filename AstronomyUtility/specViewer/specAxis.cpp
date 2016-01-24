#include <QDebug>
#include "specAxis.h"
#include "math.h"

specAxis::specAxis()
{
    m_Size=-1;
    m_Empty=true;
}

void specAxis::setxdata(double *data, int size)
{
    if(size<=0) return;
    if(!data) return;
    m_Data = (double *)malloc(size*sizeof(double));
    for(int i=0; i<size; i++) m_Data[i] = data[i];
    m_Size = size;
    m_Minimum = min_element64(m_Data,m_Size);
    m_Maximum = max_element64(m_Data,m_Size);
//    m_Lowest = m_Data[0]; //Maybe m_Uppest<m_Lowest, it doesn't matter.
//    m_Uppest = m_Data[size-1];
//    m_Lower  = m_Lowest;
//    m_Upper  = m_Uppest;
    /*Calculate x axis' gradient.*/
    m_Gradient = getGradient(m_Minimum,m_Maximum); //Because x axis is always monotone increasing/decreasing.
    /*Calculate x axis' grid range.*/
    setDataRange(m_Minimum,m_Maximum);
    m_Empty = false;
    return;
}

void specAxis::setydata(double *data, int size)
{
    if(size<=0) return;
    if(!data) return;
    m_Data = (double *)malloc(size*sizeof(double));
    for(int i=0; i<size; i++) m_Data[i] = data[i];
    m_Size = size;
    m_Minimum = min_element64(m_Data,m_Size);
    m_Maximum = max_element64(m_Data,m_Size);
//    m_Lowest = m_Data[0];
//    m_Uppest = m_Data[size-1];
//    m_Lower  = m_Lowest;
//    m_Upper  = m_Uppest;
    /*Calculate y axis' gradient.*/
    m_Gradient = getGradient(m_Minimum,m_Maximum);
    /*Calculate y axis' grid range.*/
    setDataRange(m_Minimum,m_Maximum);
    m_Empty = false;
    return;
}

double specAxis::getGradient(double min, double max)
{
    //if min=35.614, max=22.614, then gradient=10, subgradient=1.
    //if min=35.614, max=35.836, then gradient=0.1, subgradient=0.01.
    double value = fabs(max - min);
    double valued = log10(value);
    int    valuei = (int)valued;
    if(valued < -0.0) valuei--;
    return pow(10,(double)valuei);
}

double specAxis::getRange1(double lower)
{
    //if low=35.614, m_Gradient=10, then range1=30, range2=40.
    //if low=35.614, m_Gradient=0.01, then range1=35.61, range2=35.62.
    //range1,range2一定要是大刻度.
    int valuei = (int)(lower/m_Gradient);
//    if(lower<0.0) valuei--;
    valuei--;
    return (double)valuei*m_Gradient;
}

double specAxis::getRange2(double upper)
{
    int valuei = (int)(upper/m_Gradient);
//    if(upper>0.0) valuei++;
    valuei++;
    return (double)valuei*m_Gradient;
}

void specAxis::setDataRange(double lower, double upper)
{
    if(m_Size<=0) return;
    m_Range1 = getRange1(lower);
    m_Range2 = getRange2(upper);
    m_Increment = m_Range2-m_Range1;
    double cx = m_Range1;
    double dx = 0.1*m_Gradient;
    Lines.clear();
    MainLines.clear();
    MainLinesString.clear();
    while(cx<=m_Range2)
    {
        //先画出长刻度线 MainLines.
        Lines.append(QPointF(cx, 0));
        Lines.append(QPointF(cx, 16));  //长刻度线长度是16px.
        MainLines.append(QPointF(cx, 0)); //<TODO: 0? -5?>
        if(fabs(cx)<1e-12) MainLinesString.append(QString("0"));//处理一下C语言对0.0的误差.
        else MainLinesString.append(QString("%1").arg(cx));
        cx+=dx;
        if(cx>=m_Range2) break;
        //再画出短刻度线 Lines.
        for(int j=1; j<=9; j++)
        {
            Lines.append(QPointF(cx, 0));
            Lines.append(QPointF(cx, 8));
            cx+=dx;
        }
    }
}

double specAxis::variation()
{
    return m_Maximum-m_Minimum;
}

double specAxis::range()
{
    return m_Range2-m_Range1;
}

double specAxis::gradientrange()
{
    return m_Range2-m_Range1;
}

QVector<QPointF> specAxis::Lines2Points(int DeviceLength)
{
    double phy2pix = (double)DeviceLength/range();
    QVector<QPointF> output;
    for(int i=0; i< sizeofLines(); i++)
    {
        output.append(QPointF((Lines[i].x()-m_Range1)*phy2pix, Lines[i].y()));
    }
    return output;
}

QVector<QPointF> specAxis::MainLines2Points(int DeviceLength)
{
    double phy2pix = (double)DeviceLength/range();
    QVector<QPointF> output;
    for(int i=0; i< sizeofMainLines(); i++)
    {
        output.append(QPointF((MainLines[i].x()-m_Range1)*phy2pix, MainLines[i].y()));
    }
    return output;
}

QVector<QPointF> specAxis::Data2Points(int DeviceLength)
{
    double phy2pix = (double)DeviceLength/range();
    QVector<QPointF> output;
    output.clear();
    for(int i=0; i< sizeofData(); i++)
    {
        output.append(QPointF((m_Data[i]-m_Range1)*phy2pix, 0));
    }
    return output;
}

QString specAxis::getMainLinesString(int index)
{
    return MainLinesString[index];
}

int  specAxis::sizeofData()
{
    return m_Size;
}

int  specAxis::sizeofLines()
{
    return Lines.size();
}

int  specAxis::sizeofMainLines()
{
    return MainLines.size();
}

double *specAxis::data()
{
    return m_Data;
}

bool specAxis::isEmpty()
{
    return m_Empty;
}

double specAxis::max_element64(double *data, int size)
{
    int i=0;
    double max=data[0];
    for(i=0;i<size;i++)
        if(data[i]>max) max=data[i];
    return max;
}

double specAxis::min_element64(double *data, int size)
{
    int i=0;
    double min=data[0];
    for(i=0;i<size;i++)
        if(data[i]<min) min=data[i];
    return min;
}
