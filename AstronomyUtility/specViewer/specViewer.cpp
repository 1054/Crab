#include "specViewer.h"
#include "ui_specViewer.h"
#include "math.h"
#include "myfitsIO.h"

specViewer::specViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::specViewer)
{
    ui->setupUi(this);
    /*Set Fonts.*/
    m_Font = this->font();
    m_FontTT = this->font();
    m_FontTT.setPointSize(m_Font.pointSize()+2);
    m_FontTT.setBold(true);
    //setFont();
    /*Set Relative Size.*/
    ui->vSpacer_top->changeSize(this->width(),4.0*m_FontTT.pointSize());
    ui->vSpacer_bottom->changeSize(this->width(),3.0*m_Font.pointSize());
    ui->hSpacer_left->changeSize(3.0*m_Font.pointSize(),this->height());
    ui->hSpacer_right->changeSize(3.0*m_Font.pointSize(),this->height());
    setTopTitle("My Spectrum Viewer");
    setTopSubtt("X - Y Diagram");
    setBottomTitle("X Title");
    setBottomSubtt("[cm -1]");
    setLeftTitle("Y Title");
    setLeftSubtt("[Jy]");
    //setRightTitle("Y 2 Title");
    //setRightSubtt("[erg s-1 cm-2]");

    //<TEST>
    openFile();
}

specViewer::~specViewer()
{
    delete ui;
}

void specViewer::openFile()
{
    QString strFilePath;
    /*Select a file. But not open it.*/
    strFilePath = QFileDialog::getOpenFileName(this,"Open File",QDir::currentPath(),"Fits(*.fits)");
    if(strFilePath.isEmpty())
    {
        errorcode = 5;       //no input
        m_FilePath.clear();
        m_FileOpened = false;
        return;
    }
    /*See whether the path is correct or not.*/
    m_FileInfo.setFile(strFilePath);
    if(!m_FileInfo.isFile())
    {
        errorcode = 52;       //m_FilePath is not a file.
        m_FilePath.clear();
        m_FileOpened = false;
        return;
    }
    if(!m_FileInfo.isReadable())
    {
        errorcode = 53;       //File is unreadable.
        m_FilePath.clear();
        m_FileOpened = false;
        return;
    }
    //<TODO:> m_FilePath, m_FileOpened
    openFITSFile(strFilePath);
}

void specViewer::openFITSFile(QString strFilePath)
{
    int     xyleng = readFitsSpectrumRows(strFilePath.toLocal8Bit().data(),"SSWD4");
    double *xydata = readFitsSpectrum(strFilePath.toLocal8Bit().data(),"SSWD4");
    if(xyleng<=0) return;
    double *m_xData = (double *)malloc(xyleng*sizeof(double));
    double *m_yData = (double *)malloc(xyleng*sizeof(double));
    for(int i=0; i<xyleng; i++)
    {
        m_xData[i] = xydata[2*i];
        m_yData[i] = xydata[2*i+1];
    }
    ui->widget->setxdata(m_xData,xyleng);
    ui->widget->setydata(m_yData,xyleng);
}

void specViewer::openASCIIFile(QString strFilePath)
{
    //
}

void specViewer::paintEvent(QPaintEvent *event)
{
    QPainter pntr(this);
    pntr.setFont(m_FontTT);
    pntr.drawText(m_TopPoint,m_TopTitle);
    pntr.setFont(m_Font);
    pntr.drawText(m_TopSubpt,m_TopSubtt);
    pntr.drawText(m_BottomPoint,m_BottomTitle);
    pntr.drawText(m_BottomSubpt,m_BottomSubtt);
    pntr.rotate(-90);
    pntr.drawText(m_LeftPoint,m_LeftTitle);
    pntr.drawText(m_LeftSubpt,m_LeftSubtt);
    pntr.rotate(-180);
    pntr.drawText(m_RightPoint,m_RightTitle);
    pntr.drawText(m_RightSubpt,m_RightSubtt);
    pntr.rotate(-90);
    QWidget::paintEvent(event);
}

void specViewer::resizeEvent(QResizeEvent *event)
{
    int temp=0;
    //这里控制各个轴大标题和小标题的位置.
    m_TopPoint = ui->vSpacer_top->geometry().center();
    m_TopPoint.setX(m_TopPoint.x() - m_TopAlign);
    m_TopSubpt = ui->vSpacer_top->geometry().center();
    m_TopSubpt.setX(m_TopSubpt.x() - m_TopSubAl);
    m_TopSubpt.setY(m_TopSubpt.y() + ui->vSpacer_top->geometry().height()/2 - 2);

    m_BottomPoint = ui->vSpacer_bottom->geometry().center();
    m_BottomPoint.setX(m_BottomPoint.x() - m_BottomAlign);
    m_BottomSubpt = ui->vSpacer_bottom->geometry().center();
    m_BottomSubpt.setX(m_BottomSubpt.x() - m_BottomSubAl);
    m_BottomSubpt.setY(m_BottomSubpt.y() + ui->vSpacer_bottom->geometry().height()/2 - 2);

    m_LeftPoint = ui->hSpacer_left->geometry().center();
    temp = m_LeftPoint.x();
    m_LeftPoint.setX(- m_LeftPoint.y() - m_LeftAlign);
    m_LeftPoint.setY(temp);
    m_LeftSubpt = ui->hSpacer_left->geometry().center();
    m_LeftSubpt.setX(- m_LeftSubpt.y() - m_LeftSubAl);
    m_LeftSubpt.setY(temp + ui->hSpacer_left->geometry().width()/2 - 2);

    m_RightPoint = ui->hSpacer_right->geometry().center();
    temp = m_RightPoint.x();
    m_RightPoint.setX(m_RightPoint.y() - m_RightAlign);
    m_RightPoint.setY(- temp);
    m_RightSubpt = ui->hSpacer_right->geometry().center();
    m_RightSubpt.setX(m_RightSubpt.y() - m_RightSubAl);
    m_RightSubpt.setY(- temp + ui->hSpacer_right->geometry().width()/2 - 2);

    QWidget::resizeEvent(event);
}

void specViewer::setFont()
{
    bool ok;
    QFont NewFont = QFontDialog::getFont(&ok);
    if(ok)
    {
        m_Font = NewFont;
        m_FontTT = NewFont;
        m_FontTT.setPointSize(m_Font.pointSize()+2);
        m_FontTT.setBold(true);
        specViewer::setFont(NewFont);
    }
    else
        errorcode=1;
}

void specViewer::setFont(const QFont &NewFont)
{
    QWidget::setFont(NewFont);
}

void specViewer::setTopTitle(QString NewText)
{
    m_TopTitle = NewText;
    QFontMetrics m_FontMetr(m_FontTT);
    m_TopAlign = m_FontMetr.width(m_TopTitle)/2;
    qDebug() << "m_TopAlign = " << m_TopAlign;
}

void specViewer::setBottomTitle(QString NewText)
{
    m_BottomTitle = NewText;
    QFontMetrics m_FontMetr(m_Font);
    m_BottomAlign = m_FontMetr.width(m_BottomTitle)/2;
    qDebug() << "m_BottomAlign = " << m_BottomAlign;
}

void specViewer::setLeftTitle(QString NewText)
{
    m_LeftTitle = NewText;
    QFontMetrics m_FontMetr(m_Font);
    m_LeftAlign = m_FontMetr.width(m_LeftTitle)/2;
    qDebug() << "m_LeftAlign = " << m_LeftAlign;
}

void specViewer::setRightTitle(QString NewText)
{
    m_RightTitle = NewText;
    QFontMetrics m_FontMetr(m_Font);
    m_RightAlign = m_FontMetr.width(m_RightTitle)/2;
    qDebug() << "m_RightAlign = " << m_RightAlign;
}

void specViewer::setTopSubtt(QString NewText)
{
    m_TopSubtt = NewText;
    QFontMetrics m_FontMetr(m_Font);
    m_TopSubAl = m_FontMetr.width(m_TopSubtt)/2;
    qDebug() << "m_TopSubAl = " << m_TopSubAl;
}

void specViewer::setBottomSubtt(QString NewText)
{
    m_BottomSubtt = NewText;
    QFontMetrics m_FontMetr(m_Font);
    m_BottomSubAl = m_FontMetr.width(m_BottomSubtt)/2;
    qDebug() << "m_BottomSubAl = " << m_BottomSubAl;
}

void specViewer::setLeftSubtt(QString NewText)
{
    m_LeftSubtt = NewText;
    QFontMetrics m_FontMetr(m_Font);
    m_LeftSubAl = m_FontMetr.width(m_LeftSubtt)/2;
    qDebug() << "m_LeftSubAl = " << m_LeftSubAl;
}

void specViewer::setRightSubtt(QString NewText)
{
    m_RightSubtt = NewText;
    QFontMetrics m_FontMetr(m_Font);
    m_RightSubAl = m_FontMetr.width(m_RightSubtt)/2;
    qDebug() << "m_RightSubAl = " << m_RightSubAl;
}
