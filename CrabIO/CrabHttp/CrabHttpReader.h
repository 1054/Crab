/*
 * try to read part of file via Http
 *
 * compile:
 *     clang++ CrabHttpReader.cpp test.cpp -I/usr/include/qt5/ -fPIC
 */

#ifndef CRABHTTPREADER_H
#define CRABHTTPREADER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QtGui/QProgressBar>
#include <QtGui/QApplication>

class CrabHttpReader : public QObject
{
    Q_OBJECT
public:
    explicit CrabHttpReader(QObject *parent = 0);
    CrabHttpReader(QString strUrl);
    long ReadFileSize(QString strUrl = QString());

signals:

private slots:
    void ReadFileSizeSlot();

private:
    QNetworkAccessManager *httpNAM;
    QNetworkReply* httpREP;
    bool HasReply;
    QUrl httpUrl;

};

#endif // CRABHTTPREADER_H
