#ifndef CRABFITSIMAGEWIDGET_H
#define CRABFITSIMAGEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QList>
#include "CrabFitsIO.h"
#include "CrabFitsImageUtilities.h"
#include "CrabBlobItem.h"

class CrabFitsImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CrabFitsImageWidget(QWidget *parent = nullptr);
    void goSetFitsImage(double *InputFitsImage, long InputFitsImageDimension[2]);
    void goSetBlobList(QList<CrabBlobItem *> InputBlobList);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    double *FitsImage;
    long    FitsImageDimension[2];
    char   *FitsHeader;
    long   *FitsHeaderSize;
    QImage  FitsImageQImage;
    QList<CrabBlobItem *> BlobList;
};

#endif // CRABFITSIMAGEWIDGET_H
