#include "fitsImagePainter.h"

fitsImagePainter::fitsImagePainter(QWidget *parent) :
    QWidget(parent)
{
}

void fitsImagePainter::paintEvent(QPaintEvent *event)
{
    if(!m_Image.isNull())
    {
        QPainter pntr(this);
        QSize sz1 = this->size();
        //QSize sz2 = m_Image.size();
        QSize sz3 = d_Image.size();
        pntr.drawImage(QPoint((sz1.width()-sz3.width())/2,(sz1.height()-sz3.height())/2),d_Image);
    }
    QWidget::paintEvent(event);
}


void fitsImagePainter::resizeEvent(QResizeEvent *event)
{
    if(!m_Image.isNull())
    {
        QSize sz1 = this->size();
        QSize sz2 = m_Image.size();
        sz2.scale(sz1.width(),sz1.height(),Qt::KeepAspectRatio);
        if(sz1.width() > sz2.width()) //blank at left and right
            d_Image = m_Image.scaledToHeight(sz2.height());
        else //blank at up and down
            d_Image = m_Image.scaledToWidth(sz2.width());
    }
    QWidget::resizeEvent(event);
}


void fitsImagePainter::setImage(QImage InputImage)
{
    m_Image = InputImage;
    QSize sz1 = this->size();
    QSize sz2 = InputImage.size();
    sz2.scale(sz1.width(),sz1.height(),Qt::KeepAspectRatio);
    if(sz1.width() > sz2.width()) //blank at left and right
        d_Image = InputImage.scaledToHeight(sz2.height());
    else //blank at up and down
        d_Image = InputImage.scaledToWidth(sz2.width());
}

void fitsImagePainter::savImage(QString FilePath, const char *FileType)
{
    m_Image.save(FilePath,FileType);
}
