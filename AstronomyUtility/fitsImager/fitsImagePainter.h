#ifndef FITSIMAGEPAINTER_H
#define FITSIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QImage>

class fitsImagePainter : public QWidget
{
    Q_OBJECT
public:
    explicit fitsImagePainter(QWidget *parent = 0);
    void  setImage(QImage InputImage);
    void  savImage(QString FilePath, const char *FileType);


protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    //void mouseMoveEvent(QMouseEvent *);
    //void mousePressEvent(QMouseEvent *);
    //void mouseReleaseEvent(QMouseEvent *);

private:
    QImage   m_Image;
    QImage   d_Image;

};

#endif // FITSIMAGEPAINTER_H
