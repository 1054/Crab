#include "fitsImager.h"
#include "ui_fitsImager.h"
#include "CrabFitsIO.h"

fitsImager::fitsImager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fitsImager)
{
    ui->setupUi(this);
    m_ScreenBt = 0.0;
    m_ScreenCt = 1.0;
    errorcode=0;
    this->setAutoFillBackground(false);
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
    //connect(ui->openButton,SIGNAL(clicked()),this,SLOT(openFile()));
    //connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveFile()));
    //connect(ui->funcButton,SIGNAL(clicked()),this,SLOT(close()));

    /* Setup recent file list under the openButton */
    QMenu *menuRecentFiles = new QMenu(ui->openButton);
    ui->openButton->setMenu(menuRecentFiles);
    setupOpenFileMenu();

    /* Setup QPushButton stylesheet */
    ui->openButton->setStyleSheet("QPushButton:pressed{background-color:#3C3C3C;}");
    ui->closeButton->setStyleSheet("QPushButton:pressed{background-color:#3C3C3C;}");

}

fitsImager::~fitsImager()
{
    delete ui;
}


void fitsImager::setupOpenFileMenu()
{
    /* Read recent file list from ini */
    QSettings setIni(QSettings::IniFormat,QSettings::UserScope,"crabsoftware","fitsImager");
    setIni.beginGroup("fitsImager");
    QStringList listRecentFiles = setIni.value("RecentFiles").toStringList();
    // QStringList listRecentTimes = setIni.value("RecentTimes").toStringList();
    /* Setup recent file list under the openButton */
    QMenu *menuRecentFiles = ui->openButton->menu();
    menuRecentFiles->addSeparator();
    for(int i=0; i<listRecentFiles.count(); i++) {
        QAction *actionOpenRecentFile = new QAction(listRecentFiles.at(i));
        menuRecentFiles->addAction(actionOpenRecentFile);
        connect(actionOpenRecentFile,SIGNAL(triggered(bool)),this,SLOT(openFile()));
    }
}


void fitsImager::openFile()
{
    /* Check sender */
    QAction* actionOpenFile = qobject_cast<QAction*>(sender());
    if(actionOpenFile) {
       if(!actionOpenFile->text().isEmpty()) {
           // Sender is an action with file path as text, then open the file.
           openFile(actionOpenFile->text());
           return;
       }
       // Otherwise when sender is not an action,
       // or sender is an action but without file path as text,
       // then open the open file dialog.
    }
    /*Select a file. But not open it.*/
    QString strFilePath;
    strFilePath=QFileDialog::getOpenFileName(this,"Open A FITS",QDir::currentPath(),"Fits(*.fits)");
    /*Check input filepath.*/
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
    /*Open File.*/
    openFile(strFilePath);
    return;
}


void fitsImager::openFile(QString strFilePath)
{
    FilePath=strFilePath;

    /*Check fits header.*/
    int xtenumb = readFitsHeaderExtensions(FilePath.toLocal8Bit().data());
    qDebug()<<"filepwd"<<FilePath.toLocal8Bit().data();
    qDebug()<<"xtenumb"<<xtenumb;

    int xtension = 0; // <TODO> define extension

    /*Read fits header.*/
    char *HeaderText=NULL;
    long HeaderPosition=0;
    long HeaderByteSize=0;
    int errorcode = readFitsHeader(FilePath.toLocal8Bit().data(), xtension, &HeaderText, &HeaderPosition, &HeaderByteSize);
    if (errorcode!=0) { qDebug() << "fitsImager::openFile() Error! readFitsHeader failed!"; return;}
    qDebug() << "HeaderText" << (double)strlen(HeaderText)/80.0 << "HeaderPosition" << HeaderPosition << "HeaderByteSize" << HeaderByteSize;
    m_HeaderData = HeaderText;
    //m_HeaderData = readFitsHeader(FilePath.toLocal8Bit().data());
    //Note:  readFitsHeader returns a char array having a malloc size of (m_HeaderSize+1)*sizeof(char),
    //Note:  in which the last 1 byte is 0x00. Thus m_HeaderSize==strlen(m_Data).
    m_HeaderSize = strlen(m_HeaderData);
    m_BITPIX = extKeyword("BITPIX",m_HeaderData);
    m_NAXIS1 = extKeyword("NAXIS1",m_HeaderData);
    m_NAXIS2 = extKeyword("NAXIS2",m_HeaderData);
    QString strNAXIS1 = QString(m_NAXIS1).trimmed();
    QString strNAXIS2 = QString(m_NAXIS2).trimmed();
    m_Width = strNAXIS1.toInt();
    m_Height = strNAXIS2.toInt();
    m_Size = m_Width*m_Height;
    qDebug()<<"m_BITPIX"<<m_BITPIX;
    qDebug()<<"m_Width "<<m_Width;
    qDebug()<<"m_Height"<<m_Height;

    /*Read fits image data.*/
    m_Data = readFitsImage(FilePath.toLocal8Bit().data(),xtension);
    // qDebug()<<"m_Data(200,207)"<<takePixel(m_Data,200,207,m_Width,m_Height); // check passed! very good!
    // qDebug()<<"m_Data(201,207)"<<takePixel(m_Data,201,207,m_Width,m_Height); // check passed! very good!
    // qDebug()<<"m_Data(201,208)"<<takePixel(m_Data,201,208,m_Width,m_Height); // check passed! very good!

    /*Convert image data to a QImage.*/
    m_Image_mirrored = data2qimage(m_Data,m_Data_uchar,m_Width,m_Height,"log");

    /*Mirror the image along the vertical direction.*/
    // qDebug() << "m_Image dimension? " << m_Image_mirrored.width() << m_Image_mirrored.height();
    // qDebug() << "m_Image is gray? " << m_Image_mirrored.allGray();
    // qDebug() << "<TODO> m_Data_uchar" << m_Data_uchar << QString("x").prepend(strNAXIS1).prepend("(").append(strNAXIS2).append(")");
    m_Image = m_Image_mirrored.mirrored(false,true);
    // m_Image_mirrored.save("/Users/dliu/Programming/QtProgram/AstronomyUtility/fitsImager/tmp.png");
    // free(m_Data_uchar);  // <TODO> Under Mac System do not free??? <20140322>
    // m_Data_uchar=NULL;

    /*Record into ini.*/
    QSettings setIni(QSettings::IniFormat,QSettings::UserScope,"crabsoftware","fitsImager");
    setIni.beginGroup("fitsImager");
    QStringList listRecentFiles = setIni.value("RecentFiles").toStringList();
    QStringList listRecentTimes = setIni.value("RecentTimes").toStringList();
    // qDebug() << "listRecentFiles" << listRecentFiles;
    // qDebug() << "listRecentTimes" << listRecentTimes;
    // listRecentFiles.clear();
    // listRecentTimes.clear();
    // Check duplication
    if(listRecentFiles.indexOf(FilePath)>=0) {
        int indexDuplicatedRecentFile = listRecentFiles.indexOf(FilePath);
        listRecentFiles.removeAt(indexDuplicatedRecentFile);
        listRecentTimes.removeAt(indexDuplicatedRecentFile);
        listRecentFiles.prepend(FilePath);
        listRecentTimes.prepend(QDateTime::currentDateTime().toString());
    } else {
        listRecentFiles.prepend(FilePath);
        listRecentTimes.prepend(QDateTime::currentDateTime().toString());
    }
    // qDebug() << "listRecentFiles" << listRecentFiles;
    // qDebug() << "listRecentTimes" << listRecentTimes;
    if(listRecentFiles.length()>15) listRecentFiles.removeLast();
    if(listRecentTimes.length()>15) listRecentTimes.removeLast();
    setIni.setValue("RecentFiles",listRecentFiles);
    setIni.setValue("RecentTimes",listRecentTimes);
    setIni.endGroup();

    /*Display in the widget.*/
    ui->widget->setImage(m_Image);
    ui->label->setText(FilePath);
    // ui->label->setText(FilePath.prepend("<font face=\"Cantarell;Sans\">").append("</font>"));
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
    saveDialog->setNameFilter(tr("*.png *.jpg"));
    if(saveDialog->exec()==QDialog::Accepted)
    {
        QString savePath = saveDialog->selectedFiles()[0];
        QString saveType = saveDialog->selectedNameFilter();
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


void fitsImager::wheelEvent(QWheelEvent *event)
{
    /*record scroll distance.*/
    qDebug() << "wheelEvent" << event->buttons();
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;
    /*perform wheel actions.*/
    if (!numPixels.isNull()) {
        qDebug() << "wheelEvent numPixels" << numPixels;
#ifdef Q_OS_DARWIN
        qDebug() << "Q_OS_DARWIN";
        // if it is mac os then wheel action corresponds to moving the image <TODO>
        // numPixels.x(); +=left -=right
        // numPixels.y(); +=up   -=down
#endif
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        qDebug() << "wheelEvent numSteps" << numSteps;
    }
    /*event accept.*/
    event->accept();
//    QWidget::wheelEvent(event);
}


void fitsImager::mousePressEvent(QMouseEvent *event)
{
    /*zoom in zoom out.*/
    qDebug() << "mousePressEvent" << event->button();
    //if(event->button())
    QWidget::mousePressEvent(event);
}










QImage fitsImager::data2qimage(double *data, uchar *data_uchar, int width, int height, const char *mode=NULL)
{
    data_uchar = data2image(data, width*height, mode);
    QImage qimg = QImage(data_uchar, width, height, QImage::Format_ARGB32);
    return qimg;
}


uchar *fitsImager::data2image(double *data, int width, int height, const char *mode=NULL)
{
    return data2image(data,width*height,mode);
}


uchar *fitsImager::data2image(double *data, int size, const char *mode=NULL)
{
    int i=0,j=0;
    uchar *img;
    QString smode(mode);
    img = (uchar *)malloc(4*size*sizeof(uchar)); // 4bytes 32bits  QImage::Format_ARGB32
    double pixvalue = 0.0;
    double maxvalue = max_element(data,size);
    double minvalue = min_element(data,size);
    double scalefactor = 0.0;
    double intercept   = 0.0;
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
        double zerobase = minvalue>0.0?-minvalue*0.999:-minvalue*1.001; if(minvalue==0.0) {zerobase=maxvalue*0.000001;}
        scalefactor = 254.5/(double)(log10(maxvalue+zerobase));
        intercept = 0.0;//-scalefactor*(double)log10(minvalue+zerobase);
        qDebug() << "scalefactor =" << scalefactor << "intercept =" << intercept << "zerobase =" << zerobase;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = (double)(scalefactor*log10(data[j]+zerobase) + intercept); if(pixvalue<0.0) {pixvalue=0.0;}
            img[i]   = (uchar)(pixvalue); //blue   0x00=None 0xFF=Blue
            img[i+1] = (uchar)(pixvalue); //green  0x00=None 0xFF=Green
            img[i+2] = (uchar)(pixvalue); //red    0x00=None 0xFF=Red
            img[i+3] = 0xFF; //alpha  0x00=transparent 0xFF=non-transparent
            //when QImage reads uchar * on a little-endian pc, the order is inversed.
            //<TODO: big-endian, swap bytes!>
            //<TODO> debug
            //if(fabs(data[j]+42.9895)<1e-5){
            //    qDebug()<<"data[j]"<<data[j]<<"log10(data[j]+zerobase)"<<log10(data[j]+zerobase)<<"pixvalue"<<pixvalue<<"img[i]"<<img[i];
            //}
        }
    }
    else if(smode.startsWith("anti-log"))
    {
        double zerobase = minvalue>0.0?-minvalue*0.999:-minvalue*1.001; if(minvalue==0.0) {zerobase=maxvalue*0.000001;}
        scalefactor = 254.5/(double)(-log10(maxvalue+zerobase));//anti-color
        intercept = 254.5;
        for(i=0,j=0;i<4*size;i+=4,j++)
        {
            pixvalue = (double)(scalefactor*log10(data[j]+zerobase) + intercept); if(pixvalue<0.0) {pixvalue=0.0;}
            img[i]   = (uchar)(pixvalue);
            img[i+1] = (uchar)(pixvalue);
            img[i+2] = (uchar)(pixvalue);
            img[i+3] = 0xFF;
        }
    }
    return img;
}



double  fitsImager::max_element(double *data, int size)
{
    int i=0;
    double max=data[0];
    for(i=0;i<size;i++)
        if(data[i]>max) max=data[i];
    return max;
}


double  fitsImager::min_element(double *data, int size)
{
    int i=0;
    double min=data[0];
    for(i=0;i<size;i++)
        if(data[i]<min) min=data[i];
    return min;
}
