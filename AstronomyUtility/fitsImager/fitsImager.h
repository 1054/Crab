#ifndef FITSIMAGER_H
#define FITSIMAGER_H

#include <QWidget>
#include <QString>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QDesktopServices>
#include <QMimeData>
#include <QAction>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <math.h>

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

public:
    void openFile(QString strFilePath);

private:
    Ui::fitsImager *ui;
    char   *m_HeaderData;
    int     m_HeaderSize;
    char   *m_BITPIX;
    char   *m_NAXIS1;
    char   *m_NAXIS2;
    double *m_Data;
    uchar  *m_Data_uchar;
    int     m_Size;
    int     m_Width;
    int     m_Height;
    QImage  m_Image;
    QImage  m_Image_mirrored;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    QImage data2qimage(double *data, uchar *data_uchar, int width, int height, const char *mode);
    uchar *data2image(double *data, int size, const char *mode);
    uchar *data2image(double *data, int width, int height, const char *mode);
    double  max_element(double *data, int size);
    double  min_element(double *data, int size);
    double  m_ScreenBt;
    double  m_ScreenCt;
};

#endif // FITSIMAGER_H
