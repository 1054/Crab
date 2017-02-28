/* CrabFits Updated 2012-09-16 */
//
#ifndef MYFITSIO_H
#define MYFITSIO_H

/*readFitsHeader.cpp*/
char *readFitsHeader(const char *FilePath);
char *readFitsHeader(const char *FilePath, int xtension);
char *extKeyword(const char *KeywordName, const char *HeaderText);
int modKeyword(const char *strKeyName, const char *strKeyValue, const char *strHeader, int *errorcode);
int addComment(const char *strComment, const char *strHeader, int *errorcode);
int addHistory(const char *strHistory, const char *strHeader, int *errorcode);
int isENDmark(char *baLine);
int isXTENSION(char *baLine);
int isComment(char *baLine);
int isHistory(char *baLine);
int isSimpleText(char *str8);
int isSimpleText(char *str8, int length);
int isBlankSpace(char *str8);
int locateQuote(char *baLine, int nStart, int nEnd);
int locateSlash(char *baLine, int nStart, int nEnd);
char *strtrim2(const char *str);

/*readFitsImage.cpp*/
double  *readFitsImage(const char *chrFilePath);
float   *readFitsImageF(const char *chrFilePath, int ImageWidth, int ImageHeight, int HeaderSize);
double  *readFitsImageD(const char *chrFilePath, int ImageWidth, int ImageHeight, int HeaderSize);
int     *readFitsImageI32(const char *chrFilePath, int ImageWidth, int ImageHeight, int HeaderSize);
int isLittleEndian2();

/*readFitsBintable*/
int   readFitsHeaderExtensions(const char *FilePath);
int   readFitsHeader(const char *FilePath, char **HeaderText);
int   readFitsHeader(const char *FilePath, int xtension, char **HeaderText, int *HeaderPosition, int *HeaderSize);
int   extKeyword(const char *KeywordName, const char *HeaderText, char **KeywordValue);
int   readFitsXTENSION(const char *FilePath, const char* EXTNAME);
//readFitsXTENSION根据输入的BINTABLE的EXTENSION的名称EXTNAME和FITS文件来返回该EXTENSION的序号, 从0开始. 若失败则返回负数.
void *readFitsBintable(const char *FilePath, int XTENSION, int *DataXSize, int *DataYSize);
void *readFitsBintable(const char *FilePath, const char* EXTNAME, int *DataXSize, int *DataYSize);
int   readFitsBintable(const char *FilePath, int xtension, void **Data, char **UnitFormat, int *UnitSize, int *Length);
int   readFitsImage(const char *chrFilePath, int xtension, char *ImageData, int *Width, int *Height);
char *inverseBytes(char *content, int length);

/*readFitsSpectrum*/
int     readFitsSpectrumRows(const char *FilePath, const char *EXTNAME);
double *readFitsSpectrum(const char *FilePath, const char* EXTNAME);

/*writeFits*/
int writeFitsF(float *data, char *header, char *strFilePath, int *errorcode);
int writeFitsD(double *data, char *header, char *strFilePath, int *errorcode);
int writeFitsFS(float *data, int DataWidth, int DataHeight, char *strFilePath, int *errorcode);
int writeFitsDS(double *data, int DataWidth, int DataHeight, char *strFilePath, int *errorcode);


#endif // MYFITSIO_H
