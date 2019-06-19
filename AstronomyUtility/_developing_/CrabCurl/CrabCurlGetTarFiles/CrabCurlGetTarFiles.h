#ifndef CRABCURLGETTARFILES_H
#define CRABCURLGETTARFILES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <pthread.h>

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QTest>
#include <QDebug>

namespace Ui {
class CrabCurlGetTarFiles;
}


struct CurlWriteBufferStruct {
    char *buffer;
    size_t size;
}; // https://curl.haxx.se/libcurl/c/getinmemory.html

struct CurlRemoteTarFileStruct {
    QString TarFileURL;
    const char *TarFileName;
    size_t TarFileSize;
    CURL *curl_handle;
    struct CurlWriteBufferStruct *write_buffer;
    size_t byte_offset;
    int error;
};


class CrabCurlGetTarFiles : public QWidget
{
    Q_OBJECT

public:
    explicit CrabCurlGetTarFiles(QWidget *parent = 0);
    ~CrabCurlGetTarFiles();
    void openRemoteTarFile(QString url);
    static void *readRemoteTarFile(void *userdata);
    static size_t writeBuffer(char *ptr, size_t size, size_t nmemb, void *userdata); // https://curl.haxx.se/libcurl/c/getinmemory.html
    void getRemoteTarFileContent();
    void getRemoteTarFileSize();

private:
    Ui::CrabCurlGetTarFiles *ui;
    QString m_URL;
    CURL *m_CURL;
    struct BufferStruct *m_Buffer;
    QStringList m_FileNames; // files that are inside the Tar file.
    QList<size_t> m_FileSizes;
    QList<size_t> m_FileByteOffsets;
};

#endif // CRABCURLGETTARFILES_H
