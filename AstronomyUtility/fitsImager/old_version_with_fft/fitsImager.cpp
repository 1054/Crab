#include "fitsImager.h"
#include "ui_fitsImager.h"
#include "myfitsIO.h"
#include "myfitsImageFFTW.h"
#include "malloc.h"
#include "math.h"

fitsImager::fitsImager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fitsImager)
{
    ui->setupUi(this);
    m_ScreenBt = 0.0;
    m_ScreenCt = 1.0;
    errorcode=0;
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->openButton,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveFile()));
    connect(ui->FFTButton,SIGNAL(clicked()),this,SLOT(fft()));
}

fitsImager::~fitsImager()
{
    delete ui;
}


void fitsImager::openFile()
{
    QString strFilePath;
    /*Select a file. But not open it.*/
    strFilePath=QFileDialog::getOpenFileName(this,"Open A FITS",QDir::currentPath(),"Fits(*.fits)");
    /*See whether the path is correct or not.*/
    if (!strFilePath.isNull())
    {
        //QMessageBox::information(this,"Document","Has document",QMessageBox::Ok|QMessageBox::Cancel);
        FileInfo.setFile(strFilePath);
        if(!FileInfo.isFile())
        {
            errorcode = 52;       //FilePath is not a file.
            FilePath.clear();
            FileOpened = false;
            ui->label->setText("File should be a *.fits . Click open button to open a fits file.");
            return;
        }
        if(!FileInfo.isReadable())
        {
            errorcode = 53;       //File is unreadable.
            FilePath.clear();
            FileOpened = false;
            ui->label->setText("File unreadable. Click open button to open a fits file.");
            return;
        }
    }
    else
    {
        QMessageBox::information(this,"Document","No document",QMessageBox::Ok|QMessageBox::Cancel);
        return;
    }
    openFile(strFilePath);
    return;
}


void fitsImager::openFile(QString strFilePath)
{
    QImage m_Image_mirrored;
    FilePath=strFilePath;
    /*Read fits header.*/
    m_HeaderData = readFitsHeader(FilePath.toLocal8Bit().data(), &errorcode);
    //Note:  readFitsHeader returns a char array having a malloc size of (m_HeaderSize+1)*sizeof(char),
    //Note:  in which the last 1 byte is 0x00. Thus m_HeaderSize==strlen(m_Data).
    m_HeaderSize = strlen(m_HeaderData);
    m_BITPIX = extKeyword("BITPIX",m_HeaderData,&errorcode);
    m_NAXIS1 = extKeyword("NAXIS1",m_HeaderData,&errorcode);
    m_NAXIS2 = extKeyword("NAXIS2",m_HeaderData,&errorcode);
    QString strBITPIX = QString(m_BITPIX).trimmed();
    QString strNAXIS1 = QString(m_NAXIS1).trimmed();
    QString strNAXIS2 = QString(m_NAXIS2).trimmed();
    m_Width = strNAXIS1.toInt();
    m_Height = strNAXIS2.toInt();
    m_Size = m_Width*m_Height;
    qDebug()<<"m_BITPIX"<<m_BITPIX;
    qDebug()<<"m_Width "<<m_Width;
    qDebug()<<"m_Height"<<m_Height;
    /*Read fits image data.*/
    if(strBITPIX.startsWith("-32"))
    {
        m_Data = readFitsImageF(FilePath.toLocal8Bit().data(),m_Width,m_Height,m_HeaderSize,&errorcode);
    }
    /*Convert image data to a QImage.*/
    m_Image_mirrored = data2qimage32(m_Data,m_Data_uchar,m_Width,m_Height,"log");
    /*Mirror the image along the vertical direction.*/
    qDebug() << "m_Image is gray? " << m_Image_mirrored.allGray();
    m_Image = m_Image_mirrored.mirrored(false,true);
    free(m_Data_uchar);
    m_Data_uchar=NULL;
    /*Set FFTed status.*/
    m_FFTed = false;
    /*Display in the widget.*/
    ui->widget->setImage(m_Image);
    ui->label->setText(FilePath);
    ui->widget->update();
    this->update();
    return;
}


void fitsImager::saveFile()
{
    QFileDialog *saveDialog = new QFileDialog(this);
    saveDialog->setLabelText(QFileDialog::Accept,QString("Save"));
    saveDialog->setWindowTitle("Save Image");
    saveDialog->setDirectory(".");
    saveDialog->setFilter(tr("PNG(*.png);;JPG(*.jpg)"));
    if(saveDialog->exec()==QDialog::Accepted)
    {
        QString savePath = saveDialog->selectedFiles()[0];
        QString saveType = saveDialog->selectedFilter();
        QDir    qdirPath = QDir(savePath);
        /*saving path.*/
        savePath = qdirPath.absolutePath();
        /*saving type.*/
        saveType = saveType.left(3);
        /*saving image.*/
        bool saveStat = m_Image.save(savePath.append(".").append(saveType),saveType.toLocal8Bit().data());
        /*saved directory.*/
        int qurlMark = savePath.lastIndexOf("/");
        savePath = savePath.left(qurlMark);
        savePath = savePath.prepend("file:///");
        if(!saveStat)
            errorcode=56;
        else
            QDesktopServices::openUrl(QUrl(savePath));
    }
    //<TODO:>save current on-screen image?
    ui->widget->savImage(QString("TestCurrentImage.PNG"),"PNG");
    return;
}


void fitsImager::fft()
{
    int i=0;
    if(false==m_FFTed) //FT
    {
        /*Execute fourier transform.*/
        m_Data_FFTed = imageFT32(m_Data,m_Width,m_Height);
        /*Convert fftw_complex to float data.*/
        float *TempData = (float *)malloc(m_Size*sizeof(float));
        for(i=0;i<m_Size;i++)
            TempData[i] = m_Data_FFTed[i][0];
        /*Convert data to image.*/
//        uchar *TempImageUCHAR = data2image32(TempData,m_Width,m_Height,"log");
//        QImage TempImage = QImage(TempImageUCHAR,m_Width,m_Height,QImage::Format_ARGB32);
        uchar *TempDataUCHAR=NULL;
        QImage TempImage = data2qimage32(TempData,TempDataUCHAR,m_Width,m_Height,"linear");
        /*Set device image.*/
        ui->widget->setImage(TempImage.mirrored(false,true));
        /*Set FFTed status.*/
        m_FFTed = true;
        /*clean*/
        free(TempDataUCHAR);
        TempDataUCHAR=NULL;
        free(TempData);
        TempData=NULL;
    }
    else //IFT
    {
        /*Execute fourier transform.*/
        m_Data_IFTed = imageIFT32(m_Data_FFTed,m_Width,m_Height);
        /*Convert data to image.*/
        uchar *TempDataUCHAR=NULL;
        QImage TempImage = data2qimage32(m_Data_IFTed,TempDataUCHAR,m_Width,m_Height,"log");
        /*Set device image.*/
        ui->widget->setImage(TempImage.mirrored(false,true));
        /*Set FFTed status.*/
        m_FFTed = false;
        /*Clean*/
        free(TempDataUCHAR);
        TempDataUCHAR=NULL;
    }
    ui->widget->update();
    this->update();
    return;
}


void fitsImager::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
    QWidget::dragEnterEvent(event);
}


void fitsImager::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    FilePath = urls.first().toLocalFile();
    FileInfo.setFile(FilePath);
    if(!FileInfo.isFile())
    {
        errorcode = 52;       //FilePath is not a file.
        FileOpened = false;
        FilePath.clear();
        ui->label->setText("File should be a *.fits . Click open button to open a fits file.");
        return;
    }
    if(!FileInfo.isReadable())
    {
        errorcode = 53;       //File is unreadable.
        FileOpened = false;
        FilePath.clear();
        ui->label->setText("File unreadable. Click open button to open a fits file.");
        return;
    }
    FileType = FilePath.section(".",-1); //section separated by ".", and -1 means start from the right end.
    if(!FileType.startsWith("fit"))
    {
        errorcode = 54;
        FileOpened = false;
        ui->label->setText("Filetype should be *.fit(s) . Click open button to open a fits file.");
        return;
    }
    /*open fits file.*/
    openFile(FilePath);
    QWidget::dropEvent(event);
}


QImage fitsImager::data2qimage32(float *data, uchar *data_uchar, int width, int height, const char *mode=NULL)
{
    data_uchar = data2image32(data, width*height, mode);
    QImage qimg = QImage(data_uchar, width, height, QImage::Format_ARGB32);
    return qimg;
}


uchar *fitsImager::data2image32(float *data, int width, int height, const char *mode=NULL)
{
    return data2image32(data,width*height,mode);
}


uchar *fitsImager::data2image32(float *data, int size, const char *mode=NULL)
{
    int i=0,j=0;
    uchar *img;
    QString smode(mode);
    img = (uchar *)malloc(4*size*sizeof(uchar)); //32bits  QImage::Format_ARGB32
    float pixvalue = 0.0;
    float maxvalue = max_element32(data,size);
    float minvalue = min_element32(data,size);
    float scalefactor = 0.0;
    float intercept = 0.0;
    qDebug() << "maxvalue =" << maxvalue << "minvalue =" << minvalue;
    if(smode.isEmpty() || smode.startsWith("lin"))
    {
        scalefactor = 254.5/(maxvalue-minvalue);
        intercept = -scalefactor*minvalue;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = scalefactor*data[j] + intercept;
            img[i]   = (uchar)(pixvalue);
            img[i+1] = (uchar)(pixvalue);
            img[i+2] = (uchar)(pixvalue);
            img[i+3] = 0xFF;
        }
    }
    else if(smode.startsWith("anti-lin"))
    {
        scalefactor = 254.5/(minvalue-maxvalue);//anti-color
        intercept = -scalefactor*maxvalue;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = scalefactor*data[j] + intercept;
            img[i]   = (uchar)(pixvalue);
            img[i+1] = (uchar)(pixvalue);
            img[i+2] = (uchar)(pixvalue);
            img[i+3] = 0xFF;
        }
    }
    else if(smode.startsWith("log"))
    {
        scalefactor = 254.5/(float)(log10(maxvalue)-log10(minvalue));
        intercept = -scalefactor*(float)log10(minvalue);
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = (float)(scalefactor*log10(data[j]) + intercept);
            img[i]   = (uchar)(pixvalue); //blue   0x00=None 0xFF=Blue
            img[i+1] = (uchar)(pixvalue); //green  0x00=None 0xFF=Green
            img[i+2] = (uchar)(pixvalue); //red    0x00=None 0xFF=Red
            img[i+3] = 0xFF; //alpha  0x00=transparent 0xFF=non-transparent
            //when QImage reads uchar * on a little-endian pc, the order is inversed.
            //<TODO: big-endian, swap bytes!>
        }
    }
    else if(smode.startsWith("anti-log"))
    {
        scalefactor = 254.5/(float)(log10(minvalue)-log10(maxvalue));//anti-color
        intercept = -scalefactor*(float)log10(maxvalue);
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = (float)(scalefactor*log10(data[j]) + intercept);
            img[i]   = (uchar)(pixvalue);
            img[i+1] = (uchar)(pixvalue);
            img[i+2] = (uchar)(pixvalue);
            img[i+3] = 0xFF;
        }
    }
    return img;
}



float  fitsImager::max_element32(float *data, int size)
{
    int i=0;
    float max=data[0];
    for(i=0;i<size;i++)
        if(data[i]>max) max=data[i];
    return max;
}


float  fitsImager::min_element32(float *data, int size)
{
    int i=0;
    float min=data[0];
    for(i=0;i<size;i++)
        if(data[i]<min) min=data[i];
    return min;
}
