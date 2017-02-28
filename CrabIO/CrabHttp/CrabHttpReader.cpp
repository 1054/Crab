#include "CrabHttpReader.h"

CrabHttpReader::CrabHttpReader(QObject *parent) :
    QObject(parent)
{
    this->httpUrl.clear();
    this->httpNAM = NULL;
    this->httpREP = NULL;
}

CrabHttpReader::CrabHttpReader(QString strUrl)
{
    this->httpUrl.clear();
    this->httpNAM = NULL;
    this->httpREP = NULL;
    if(!strUrl.isEmpty()) {
        this->httpUrl = QUrl(strUrl);
        qDebug() << "CrabHttpReader::CrabHttpReader() httpUrl =" << this->httpUrl;
    }
}

long CrabHttpReader::ReadFileSize(QString strUrl)
{
    long FileSize = -1;
    if(!strUrl.isNull()) {
        if(!strUrl.isEmpty()) {
            this->httpUrl = QUrl(strUrl);
        }
    }
    if(!this->httpUrl.isEmpty()) {
        qDebug() << "CrabHttpReader::ReadFileSize() httpUrl =" << this->httpUrl;
        this->HasReply = false;
        httpNAM = new QNetworkAccessManager();
        QNetworkRequest httpREQ;
        httpREQ.setUrl(this->httpUrl);
        httpREP = httpNAM->get(httpREQ);
        // connect(httpREP, SIGNAL(metaDataChanged()), this, SLOT(ReadFileSizeSlot()));
        qDebug() << "CrabHttpReader::ReadFileSize() httpUrl =" << this->httpUrl;
        while(!httpREP->isFinished()) {
            qApp->processEvents(QEventLoop::WaitForMoreEvents);
            // qDebug() << "CrabHttpReader::ReadFileSize() WaitForMoreEvents";
        }
        FileSize = httpREP->header(QNetworkRequest::ContentLengthHeader).toUInt();
        qDebug() << "CrabHttpReader::ReadFileSize() FileSize =" << FileSize;
        httpREP->deleteLater();
        httpNAM->deleteLater();
    }
    return FileSize;
}

void CrabHttpReader::ReadFileSizeSlot()
{
    if(httpREP) {
        qDebug() << "CrabHttpReader::ReadFileSizeSlot() httpREP =" << httpREP;
        this->HasReply = true;
    }
}
