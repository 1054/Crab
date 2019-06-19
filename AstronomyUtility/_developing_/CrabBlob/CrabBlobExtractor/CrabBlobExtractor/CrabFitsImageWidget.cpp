#include "CrabFitsImageWidget.h"

CrabFitsImageWidget::CrabFitsImageWidget(QWidget *parent) : QWidget(parent)
{
    FitsImage = NULL;
    FitsHeader = NULL;
    FitsImageDimension[0] = 0;
    FitsImageDimension[1] = 0;
    this->setMinimumHeight(200);
    this->setMinimumWidth(200);
}

void CrabFitsImageWidget::goSetFitsImage(double *InputFitsImage, long InputFitsImageDimension[2])
{
    FitsImage = InputFitsImage;
    FitsImageDimension[0] = InputFitsImageDimension[0];
    FitsImageDimension[1] = InputFitsImageDimension[1];
    if(FitsImage && FitsImageDimension[0]>0 && FitsImageDimension[1]>0) {
        FitsImageQImage = CrabFitsImageUtilities::data2qimage(FitsImage,FitsImageDimension);
        FitsImageQImage = FitsImageQImage.mirrored();
    }
    this->update();
    qDebug() << "CrabFitsImageWidget::goSetFitsImage()";
}

void CrabFitsImageWidget::goSetBlobList(QList<CrabBlobItem *> InputBlobList)
{
    BlobList = InputBlobList;
    this->update();
    qDebug() << "CrabFitsImageWidget::goSetBlobList()";
}

void CrabFitsImageWidget::paintEvent(QPaintEvent *event)
{
    if(FitsImage && FitsImageDimension[0]>0 && FitsImageDimension[1]>0) {
        QImage tempImage = FitsImageQImage.scaled(this->width(), this->height(), Qt::KeepAspectRatio);
        QPainter tempPainter(this);
        tempPainter.drawImage(QPoint(0,0), tempImage);
        // draw blobs
        QPen tempPen;
        tempPen.setWidth(2);
        tempPen.setBrush(Qt::green);
        tempPen.setCapStyle(Qt::RoundCap);
        tempPen.setJoinStyle(Qt::RoundJoin);
        tempPainter.setPen(tempPen);
        if(BlobList.count()>0) {
            for(int k=0; k<BlobList.count(); k++) {
                double temp_xc = BlobList.at(k)->xc / FitsImageQImage.width() * tempImage.width();
                double temp_yc = (FitsImageQImage.height() - BlobList.at(k)->yc) / FitsImageQImage.height() * tempImage.height(); // because Qt image is mirrored.
                tempPainter.drawEllipse(QPointF(temp_xc,temp_yc), 10.0, 10.0);
            }
        }
    }
    QWidget::paintEvent(event);
}
