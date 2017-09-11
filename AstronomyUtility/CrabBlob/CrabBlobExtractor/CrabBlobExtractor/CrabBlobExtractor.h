#ifndef CRABBLOBEXTRACTOR_H
#define CRABBLOBEXTRACTOR_H

#include <QWidget>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QDateTime>
#include <QSplitter>
#include <QDebug>
#include "CrabFitsIO.h"
#include "CrabBlobListWidget.h"
#include "CrabFitsImageWidget.h"
#include "blitzwave/Wavelet.h"
#include "blitzwave/WaveletDecomp.h"
#include <cmath>

namespace Ui {
class CrabBlobExtractor;
}

class CrabBlobExtractor : public QWidget
{
    Q_OBJECT

public:
    explicit CrabBlobExtractor(QWidget *parent = 0);
    ~CrabBlobExtractor();

public slots:
    int goOpenFitsImage();
    int goReadFitsImage(QString InputFilePath);
    int goExtractBlobs();
    int goClearData();
    int goClose();

protected:
    QString goReadRecentFile();
    int goSaveRecentFile(QString InputFilePath);

private:
    Ui::CrabBlobExtractor *ui;
    QString FitsFile;
    double *FitsImage;
    long    FitsImageDimension[2];
    char   *FitsHeader;
    long   *FitsHeaderSize;
    CrabFitsImageWidget *FitsImageWidget;
    CrabBlobListWidget *BlobListWidget;
};

#endif // CRABBLOBEXTRACTOR_H
