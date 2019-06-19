#include "CrabBlobExtractor.h"
#include "ui_CrabBlobExtractor.h"

using namespace blitz;
using namespace bwave;


CrabBlobExtractor::CrabBlobExtractor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrabBlobExtractor)
{
    this->FitsImage = NULL;
    this->FitsHeader = NULL;

    ui->setupUi(this);

    QSplitter *tempSplitter = new QSplitter(this);
    FitsImageWidget = new CrabFitsImageWidget(this);
    BlobListWidget = new CrabBlobListWidget(this);
    tempSplitter->addWidget(FitsImageWidget);
    tempSplitter->addWidget(BlobListWidget);

    ui->horizontalLayout_Body->addWidget(tempSplitter);

    connect(ui->pushButton_Open,SIGNAL(clicked(bool)),this,SLOT(goOpenFitsImage()));
    connect(ui->pushButton_Blobs,SIGNAL(clicked(bool)),this,SLOT(goExtractBlobs()));
    connect(ui->pushButton_Close,SIGNAL(clicked(bool)),this,SLOT(goClose()));
}

CrabBlobExtractor::~CrabBlobExtractor()
{
    delete ui;
}


int CrabBlobExtractor::goOpenFitsImage()
{
    QString tempInitialDir = QDir::homePath();
    QString tempRecentFile = this->goReadRecentFile();
    if(!tempRecentFile.isEmpty()) {
        tempInitialDir = QFileInfo(tempRecentFile).absoluteDir().absolutePath();
    }
    if(!ui->lineEdit_FitsImage->text().isEmpty()) {
        tempInitialDir = QFileInfo(ui->lineEdit_FitsImage->text()).absoluteDir().absolutePath();
    }
    QString tempFilePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                       tempInitialDir,
                                                       tr("FITS Image (*.fits *.FITS)"));
    if(!QFileInfo(tempFilePath).isFile()) {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Error! Could not find the FITS file \"")+tempFilePath+tr("\"!"),
                             QMessageBox::Ok);
        return -1;
    }

    // Save Recent Files
    goSaveRecentFile(tempFilePath);

    // Read Fits Image
    goReadFitsImage(tempFilePath);

    // Display Fits File Path
    ui->lineEdit_FitsImage->setText(tempFilePath);

    // Display Fits Image Widget
    this->FitsImageWidget->goSetFitsImage(this->FitsImage, this->FitsImageDimension);

    return 0;
}


int CrabBlobExtractor::goReadFitsImage(QString InputFilePath)
{
    // call functions in CrabFitsIO.cpp
    this->FitsFile = InputFilePath;
    this->FitsImage = readFitsImage(this->FitsFile.toLatin1().data());
    this->FitsHeader = readFitsHeader(this->FitsFile.toLatin1().data(),0);
    char *tempKeyNAXIS1 = extKeyword("NAXIS1",this->FitsHeader);
    char *tempKeyNAXIS2 = extKeyword("NAXIS2",this->FitsHeader);
    if(tempKeyNAXIS1==NULL || tempKeyNAXIS2==NULL) {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Error! Failed to read NAXIS1 and NAXIS2 from the FITS file \"")+InputFilePath+tr("\"!"),
                             QMessageBox::Ok);
        return -1;
    }
    this->FitsImageDimension[0] = atol(tempKeyNAXIS1);
    this->FitsImageDimension[1] = atol(tempKeyNAXIS2);
    qDebug() << "CrabBlobExtractor::goReadFitsImage()" << "Read" << this->FitsImageDimension[0] << "x" << this->FitsImageDimension[1] << "image data.";
    return 0;
}


int CrabBlobExtractor::goExtractBlobs()
{
    if(!this->FitsImage) {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Error! No FITS image loaded!"),
                             QMessageBox::Ok);
        return -1;
    }

    // Input Image
    // -> Background subtraction
    // -> Image filtering (convolution, mask, or retina)
    // -> Image segmentation (i.e. blob finding)

    double *tempImage = CrabFitsImageUtilities::copy_image_no_nan(FitsImage, FitsImageDimension[0], FitsImageDimension[1]);
    long iWidth = FitsImageDimension[0];
    long iHeight = FitsImageDimension[1];
    long iSize = iWidth*iHeight;

//    // ---- TODO -- Wavelet
//    // Create a blitz array on data, assuming C-style memory layout (data is used directly, not copied):
//    GeneralArrayStorage<2> temp_bwave_storage;
//    temp_bwave_storage.ordering() = secondDim, firstDim; // nrows, ncols
//    Array<double,2> temp_bwave_array(tempImage, shape(iHeight,iWidth), neverDeleteData, temp_bwave_storage);
//    // Create a CDF(2,2) nonstandard wavelet decomposition in 2 dimensions:
//    //WaveletDecomp<2> temp_bwave_decomp(WL_CDF_2_2, NONSTD_DECOMP);
//    WaveletDecomp<2> temp_bwave_decomp(WL_CDF_1_1);
//    // Apply the bwave decomposition on the data (in situ)
//    temp_bwave_decomp.apply(temp_bwave_array);
//    // ----
//    double *tempImage2 = CrabFitsImageUtilities::copy_image_no_nan(FitsImage, FitsImageDimension[0], FitsImageDimension[1]);
//    for(long j=0; j<iHeight; j++) {
//        for(long i=0; i<iWidth; i++) {
//            TinyVector<long,2> temp_bwave_index; temp_bwave_index = j,i;
//            tempImage2[j*iWidth+i] = temp_bwave_array(temp_bwave_index);
//        }
//    }
//    FitsImage = tempImage2;
//    // ----


    // Loop to find maxima and extract blob
    QList<CrabBlobItem *> tempBlobList;
    long iBlob = 0;
    while (iBlob<=1) {
        // find a global maxima
        long iFound = -1;
        double fMax = CrabFitsImageUtilities::max_element_no_nan(tempImage, iSize, &iFound); // pixel value of the found maxima
        long iMax = iFound%iWidth; // i coordinate of the found maxima
        long jMax = iFound/iWidth; // j coordinate of the found maxima
        qDebug() << "CrabBlobExtractor::goExtractBlobs()" << "iMax" << iMax << "jMax" << jMax << "fMax" << fMax;
        // extract a blob
        double max_distance = 10.0; // out to 10 pixel in radius
        QMap<QString,double> fCoG = CrabFitsImageUtilities::curve_of_growth(tempImage, iWidth, iHeight, iMax, jMax, max_distance, 0.0, 180.0);
        qDebug() << "CrabBlobExtractor::goExtractBlobs()" << "fCoG" << fCoG;
        // store blob as tempBlobItem
        CrabBlobItem *tempBlobItem = new CrabBlobItem();
        tempBlobItem->xc = iMax+1.0;
        tempBlobItem->yc = jMax+1.0;
        if(fCoG.keys().contains("0.000"))
            tempBlobItem->fc = fCoG["0.000"];
        else
            tempBlobItem->fc = NAN;
        // store blob into tempBlobList
        tempBlobList.append(tempBlobItem);
        // go for next blob
        iBlob++;
    }


    // Store blob
    this->FitsImageWidget->goSetBlobList(tempBlobList);


    // Display New Fits Image Widget
    this->FitsImageWidget->goSetFitsImage(this->FitsImage, this->FitsImageDimension);

    return iBlob;

}


int CrabBlobExtractor::goClearData()
{
    this->FitsFile = QString();
    if(this->FitsImage) {
        free(this->FitsImage);
    }
    this->FitsImage = NULL;
    this->FitsImageDimension[0] = 0;
    this->FitsImageDimension[1] = 0;
    if(this->FitsHeader) {
        free(this->FitsHeader);
    }
    this->FitsHeader = NULL;
    return 0;
}


int CrabBlobExtractor::goClose()
{
    this->goClearData();
    this->close();
    return 0;
}






QString CrabBlobExtractor::goReadRecentFile()
{
    // Read ini
    QSettings setIni(QSettings::IniFormat, QSettings::UserScope, "crabsoftware","CrabBlobExtractor");
    setIni.beginGroup("CrabBlobExtractor");
    QStringList listRecentFiles = setIni.value("RecentFiles").toStringList();
    //QStringList listRecentTimes = setIni.value("RecentTimes").toStringList();
    setIni.endGroup();
    if(listRecentFiles.size()>0) {
        qDebug() << "goReadRecentFile" << listRecentFiles.at(0);
        return listRecentFiles.at(0);
    }
    return QString();
}

int CrabBlobExtractor::goSaveRecentFile(QString InputFilePath)
{
    // Record into ini
    QSettings setIni(QSettings::IniFormat, QSettings::UserScope, "crabsoftware","CrabBlobExtractor");
    setIni.beginGroup("CrabBlobExtractor");
    QStringList listRecentFiles = setIni.value("RecentFiles").toStringList();
    QStringList listRecentTimes = setIni.value("RecentTimes").toStringList();
    // Limit RecentFile Number
    int limitRecentFile = 15;
    // Check duplication
    if(listRecentFiles.indexOf(InputFilePath)>=0) {
        int indexDuplicatedRecentFile = listRecentFiles.indexOf(InputFilePath);
        listRecentFiles.removeAt(indexDuplicatedRecentFile);
        listRecentTimes.removeAt(indexDuplicatedRecentFile);
        listRecentFiles.prepend(InputFilePath);
        listRecentTimes.prepend(QDateTime::currentDateTime().toString());
    } else {
        listRecentFiles.prepend(InputFilePath);
        listRecentTimes.prepend(QDateTime::currentDateTime().toString());
    }
    if(listRecentFiles.length()>limitRecentFile) listRecentFiles.removeLast();
    if(listRecentTimes.length()>limitRecentFile) listRecentTimes.removeLast();
    setIni.setValue("RecentFiles",listRecentFiles);
    setIni.setValue("RecentTimes",listRecentTimes);
    setIni.endGroup();
    qDebug() << "CrabBlobExtractor::goSaveRecentFile()" << listRecentFiles;
    return 0;
}

















