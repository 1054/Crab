#ifndef FITSIMAGER_H
#define FITSIMAGER_H

#include <QWidget>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QDesktopServices>
#include <QString>
#include <QDebug>
#include "fftw3.h"

namespace Ui {
    class fitsImager;
}

class fitsImager : public QWidget
{
    Q_OBJECT

public:
    explicit fitsImager(QWidget *parent = 0);
    ~fitsImager();
    int errorcode;
    bool    FileOpened;
    QString FilePath;
    QString FileType;
    QFileInfo FileInfo;

public slots:
    void openFile();
    void saveFile();
    void fft();

public:
    void openFile(QString strFilePath);

private:
    Ui::fitsImager *ui;
    char   *m_HeaderData;
    int     m_HeaderSize;
    char   *m_BITPIX;
    char   *m_NAXIS1;
    char   *m_NAXIS2;
    float  *m_Data;
    uchar  *m_Data_uchar;
    fftw_complex *m_Data_FFTed;
    float        *m_Data_IFTed;
    int     m_Size;
    int     m_Width;
    int     m_Height;
    QImage  m_Image;
    bool    m_FFTed;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    QImage data2qimage32(float *data, uchar *data_uchar, int width, int height, const char *mode);
    uchar *data2image32(float *data, int size, const char *mode);
    uchar *data2image32(float *data, int width, int height, const char *mode);
    float  max_element32(float *data, int size);
    float  min_element32(float *data, int size);
    float  m_ScreenBt;
    float  m_ScreenCt;
};

#endif // FITSIMAGER_H
