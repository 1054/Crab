//本程序根据输入的xdata,ydata算出x轴,y轴的显示范围和刻度标尺.
//Lines 指的是轴上的各个刻度线.
//MainLines 指的是轴上的主要刻度线.
//MainLinesString 指的是轴上主要刻度线的数值,这是要显示出来的刻度数值.
//
#ifndef SPECAXIS_H
#define SPECAXIS_H
#include <QString>
#include <QVector>
#include <QPointF>
class specAxis
{
public:
    specAxis();
    void setxdata(double *data, int size);
    void setydata(double *data, int size);
    void setDataRange(double lower, double upper);
    double variation();
    double range();
    double gradientrange();
    int    sizeofData();
    int    sizeofLines();
    int    sizeofMainLines();
    double *data();
    bool   isEmpty();
    QVector<QPointF> Data2Points(int DeviceLength);
    QVector<QPointF> Lines2Points(int DeviceLength);
    QVector<QPointF> MainLines2Points(int DeviceLength); //only main lines, excluded sub-gradients.
    QString getMainLinesString(int index);


private:
    QVector<QPointF> Lines;
    QVector<QPointF> MainLines;
    QVector<QString> MainLinesString;
    double *m_Data;
    int     m_Size;
    bool    m_Empty;
    QString m_Title;
    QString m_Subtitle;

//    double  m_Range0; //where pixel==0;
    double  m_Range1;
    double  m_Range2;
    double  m_Gradient;
    double  m_Increment;
    //e.g. 35.614->38.666, m_Range1=35, m_Range2=39, m_Gradient=1, m_Increment=4.
    double  m_Minimum;
    double  m_Maximum;
//    double  m_Lowest;
//    double  m_Uppest;
//    double  m_Lower;
//    double  m_Upper;

    double getGradient(double min, double max);
    double getRange1(double lower);
    double getRange2(double upper);
    double max_element64(double *data, int size);
    double min_element64(double *data, int size);

};

#endif // SPECAXIS_H
