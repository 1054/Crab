#include "CrabCurlGetTarFiles.h"
#include "ui_CrabCurlGetTarFiles.h"

CrabCurlGetTarFiles::CrabCurlGetTarFiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrabCurlGetTarFiles)
{
    ui->setupUi(this);

    m_CURL = NULL;
    m_Buffer = NULL;

    curl_global_init(CURL_GLOBAL_ALL);

    this->openRemoteTarFile("https://almascience.eso.org/dataPortal/requests/anonymous/82420841/ALMA/uid___A002_X9ff365_X175a/2013.1.00803.S_uid___A002_X9ff365_X175a.asdm.sdm.tar");
    this->getRemoteTarFileContent();

}

CrabCurlGetTarFiles::~CrabCurlGetTarFiles()
{
    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();

    delete ui;
}


size_t CrabCurlGetTarFiles::writeBuffer(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    CurlWriteBufferStruct buffer = *(CurlWriteBufferStruct *)userdata; // this is buggy, must use buffer_ptr!
    CurlWriteBufferStruct *buffer_ptr = (CurlWriteBufferStruct *)userdata;
    //qDebug() << "writeBuffer" << "buffer.buffer = " << (void *)buffer.buffer;
    //qDebug() << "writeBuffer" << "buffer_ptr->buffer = " << (void *)buffer_ptr->buffer;
    if(buffer_ptr->buffer == NULL || buffer_ptr->size == 0) {
        buffer_ptr->buffer = (char *)malloc(realsize + 1);
        memset(buffer_ptr->buffer, 0x0, realsize + 1);
        //qDebug() << "writeBuffer malloc buffer to size" << buffer_ptr->size + realsize + 1 << "bytes";
    } else {
        buffer_ptr->buffer = (char *)realloc((void *)buffer_ptr->buffer, buffer_ptr->size + realsize + 1);
        //qDebug() << "writeBuffer realloc buffer to size" << buffer_ptr->size + realsize + 1 << "bytes";
    }
    if(buffer_ptr->buffer == NULL) {
        printf("writeBuffer got error of out of memory (realloc returned NULL)!\n"); return 0;
    }
    memcpy(buffer_ptr->buffer + buffer_ptr->size, ptr, realsize);
    buffer_ptr->size += realsize;
    memset(buffer_ptr->buffer + buffer_ptr->size, 0x0, 1);
    return realsize;
} // https://curl.haxx.se/libcurl/c/getinmemory.html
  // https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html
  // https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html


void CrabCurlGetTarFiles::openRemoteTarFile(QString url)
{
    /* check previous m_CURL etc. */
    if(m_CURL) {
        curl_easy_cleanup(m_CURL);
        m_CURL = NULL;
    }
    if(m_FileNames.count()>0) {
        m_FileNames.clear();
    }
    if(m_FileSizes.count()>0) {
        m_FileSizes.clear();
    }
    if(m_FileByteOffsets.count()>0) {
        m_FileByteOffsets.clear();
    }

    /* store url */
    m_URL = url;
    qDebug() << "openRemoteTarFile" << m_URL << &m_URL << &url;
    //qDebug() << "openRemoteTarFile" << m_URL.toLatin1().data();

    /* init the curl session */
    /* -- https://stackoverflow.com/questions/3573146/problem-using-libcurl-it-does-not-appear-to-get-the-entire-page */
    //m_CURL = curl_easy_init();
}


void *CrabCurlGetTarFiles::readRemoteTarFile(void *userdata)
{
    CurlRemoteTarFileStruct *remote_tar_file_ptr = (CurlRemoteTarFileStruct *)userdata; // this is buggy!
    CurlRemoteTarFileStruct remote_tar_file = *(CurlRemoteTarFileStruct *)userdata;
    //qDebug() << "readRemoteTarFile" << "remote_tar_file.curl_handle = " << (void *)remote_tar_file.curl_handle;
    //qDebug() << "readRemoteTarFile" << "remote_tar_file_ptr->curl_handle = " << (void *)remote_tar_file_ptr->curl_handle;
    if(remote_tar_file.curl_handle) {

        /* reset curl handler and set parameters */
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_URL, remote_tar_file.TarFileURL.toLatin1().data());
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_WRITEFUNCTION, writeBuffer);
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_WRITEDATA, (void *)remote_tar_file.write_buffer);
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_TIMEOUT_MS, 15000);

        /* set byte offset */
        curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)remote_tar_file.byte_offset);

        /* get the data */
        CURLcode result = curl_easy_perform(remote_tar_file.curl_handle);
        //qDebug() << "readRemoteTarFile" << "curl_easy_perform()" << (remote_tar_file.TarFileName) << remote_tar_file.TarFileSize;
        //qDebug() << "readRemoteTarFile" << "curl_easy_perform()" << (remote_tar_file.TarFileURL) << remote_tar_file.TarFileURL << remote_tar_file.TarFileSize;
        //qDebug() << "readRemoteTarFile" << "curl_easy_perform()" << (remote_tar_file.TarFileName) << remote_tar_file.TarFileSize;
        qDebug() << "readRemoteTarFile" << "curl_easy_perform()" << (remote_tar_file.TarFileURL) << remote_tar_file.TarFileSize;

        /* check for errors */
        if(result != CURLE_OK) {
            fprintf(stderr, "readRemoteTarFile curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
            remote_tar_file_ptr->error = -1;
        } else {
            printf("readRemoteTarFile curl_easy_perform() %lu bytes retrieved\n", remote_tar_file.write_buffer->size);
            remote_tar_file_ptr->error = 0;
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void CrabCurlGetTarFiles::getRemoteTarFileContent()
{
    int i = 0, j = 0, k = 0;
    int error = 0;
    if(!m_URL.isEmpty()) {

        // TODO: while loop
        if(1) {

            /* Loop to read FileName, FileSize and FileByteOffset. They are at speicifc position in Tar file. */
            for(j = 0; j < 2; j++) {

                /* prepare struct variables */
                m_CURL = curl_easy_init();
                struct CurlWriteBufferStruct buffer;
                                             buffer.buffer = NULL;
                                             buffer.size = 0;
                struct CurlRemoteTarFileStruct remote_tar_file;
                                               remote_tar_file.curl_handle = m_CURL;
                                               remote_tar_file.write_buffer = &buffer;
                                               remote_tar_file.byte_offset = 0;
                                               remote_tar_file.TarFileURL = m_URL;
                                               remote_tar_file.TarFileName = m_URL.toLatin1().data(); // this is buggy!
                                               remote_tar_file.TarFileSize = 0;
                                               remote_tar_file.error = 0;

                /* set from which byte offset we read the data, and read how many bytes */
                size_t from_byte_offset = 0;
                size_t read_byte_length = 100;

                /* set from which byte offset we read the data, and read how many bytes */
                if(j==0) {
                    from_byte_offset = 0; // Tar file format, byte offset 0 - 99 is the file name.
                    read_byte_length = 100; // Tar file format, byte offset 0 - 99 is the file name.
                } else if(j==1) {
                    from_byte_offset = 124; // Tar file format, byte offset 124 - 135 is the file size (octal base).
                    read_byte_length = 12; // Tar file format, byte offset 124 - 135 is the file size (octal base).
                }
                remote_tar_file.byte_offset = from_byte_offset;

                /* prepare a thread to run curl separately */
                pthread_t thread_id;
                error = pthread_create(&thread_id, NULL, readRemoteTarFile, (void *)&remote_tar_file);
                if(0 != error) {
                    fprintf(stderr, "getRemoteTarFileContent couldn't run thread %d! errno %d\n", i, error); return;
                }

                /* check buffer size */
                while(buffer.size < read_byte_length && remote_tar_file.error == 0) {
                    QTest::qWait(150);
                }

                /* let CURL ends after we got the required byte length. this is done by setting CURLOPT_TIMEOUT_MS to 1ms */
                curl_easy_setopt(remote_tar_file.curl_handle, CURLOPT_TIMEOUT_MS, 1); // -- https://stackoverflow.com/questions/28767613/cancel-curl-easy-perform-while-it-is-trying-to-connect


                /* wait for thread to terminate */
                // for(i = 0; i< NumThread; i++) {
                //     error = pthread_join(list_of_thread[i], NULL);
                //     fprintf(stderr, "Thread %d ended!\n", i);
                // }
                error = pthread_join(thread_id, NULL);

                /* process what we have got */
                printf("getRemoteTarFileContent read %lu bytes: \n", buffer.size);
                if(buffer.size>=read_byte_length) {
                    char *got_text = (char *)malloc((read_byte_length+1)*sizeof(char));
                    memset(got_text,0x20,read_byte_length);
                    memset(got_text+read_byte_length,0x00,1);
                    memcpy(got_text,buffer.buffer,read_byte_length);

                    /* store what we have got */
                    if(j==0) {
                        printf("getRemoteTarFileContent extracting %lu bytes as the file name: \n", read_byte_length);
                        //printf("%s\n", got_text);
                        m_FileNames.append(QString(got_text));
                        qDebug() << "getRemoteTarFileContent" << "m_FileNames.append" << m_FileNames[m_FileNames.count()-1];
                    } else if(j==1) {
                        printf("getRemoteTarFileContent extracting %lu bytes as the file size: \n", read_byte_length);
                        //printf("%s\n", got_text);
                        m_FileSizes.append((size_t)(QString::number(atol(got_text),8).toLong()));
                        qDebug() << "getRemoteTarFileContent" << "m_FileSizes.append" << m_FileSizes[m_FileSizes.count()-1];
                    }
                } else {
                    printf("getRemoteTarFileContent failed to read %lu bytes as the file name!\n", read_byte_length);
                }


                /* free buffer */
                if(buffer.buffer) {
                    memset(buffer.buffer,0x0,buffer.size);
                    free(buffer.buffer);
                    buffer.buffer = NULL;
                    buffer.size = 0;
                }

                curl_easy_cleanup(m_CURL);

                qDebug() << "getRemoteTarFileContent" << "---------------------------------------------------";
            }
        }

        qDebug() << "getRemoteTarFileContent" << "m_FileNames" << m_FileNames;
        qDebug() << "getRemoteTarFileContent" << "m_FileSizes" << m_FileSizes;
    }
}


void CrabCurlGetTarFiles::getRemoteTarFileSize()
{
    m_CURL = curl_easy_init();
    curl_easy_cleanup(m_CURL);
    qDebug() << "getRemoteTarFileSize" << "---------------------------------------------------";
}

