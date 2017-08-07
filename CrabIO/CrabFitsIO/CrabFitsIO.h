/* CrabFits Updated 2012-09-16 */
/* CrabFits Updated 2017-07-30 addKeyword() */
/* CrabFits Updated 2017-08-03 modifyFitsImage() */
/* CrabFits Updated 2017-08-07 addKeyword() added input argument "debugcode" */
/* CrabFits Updated 2017-08-07 extKeyword() added input argument "KeepQuotes" and "debugcode" */
//
#ifndef MYFITSIO_H
#define MYFITSIO_H

/*readFitsHeader.cpp*/
char *readFitsHeader(const char *FilePath);
char *readFitsHeader(const char *FilePath, int xtension);
int   readFitsHeader(const char *FilePath, char **HeaderText);
int   readFitsHeader(const char *FilePath, int xtension, char **HeaderText, long *HeaderPosition, long *HeaderSize);
int   readFitsHeaderExtensions(const char *FilePath);
char *extKeyword(const char *KeywordName, const char *HeaderText, int KeepQuotes = 0, int debugcode = 0);
int   extKeyword(const char *KeywordName, const char *HeaderText, char **KeywordValue, int KeepQuotes = 0, int debugcode = 0);
int   modKeyword(const char *strKeyName, const char *strKeyValue, char *strHeader); // we only need to modify Header str without changing strlen, so we can use char * instead of char **.
int   addKeyword(const char *strKeyName, const char *strKeyValue, char **HeaderPointer, const char *strComment = NULL, int addEmptyLine = 2, int debugcode = 0); // we will change the Header strlen, so we need char ** instead of char *.
int   addComment(const char *strComment, const char *strHeader);
int   addHistory(const char *strHistory, const char *strHeader);
int   isENDmark(char *baLine);
int   isXTENSION(char *baLine);
int   isComment(char *baLine);
int   isHistory(char *baLine);
int   isSimpleText(char *str8);
int   isSimpleText(char *str8, int length);
int   isBlankSpace(char *str8);
int   locateQuote(char *baLine, int nStart, int nEnd);
int   locateSlash(char *baLine, int nStart, int nEnd);
char *strtrim2(const char *str);

/*readFitsImage.cpp*/
double  *readFitsImage(const char *chrFilePath, int xtension = 0);
float   *readFitsImageF(const char *chrFilePath, int ImageWidth, int ImageHeight, long HeaderSize);
double  *readFitsImageD(const char *chrFilePath, int ImageWidth, int ImageHeight, long HeaderSize);
int     *readFitsImageI32(const char *chrFilePath, int ImageWidth, int ImageHeight, long HeaderSize);
double  *readFitsImageBlock(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int xtension = 0);
float   *readFitsImageBlockF(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int ImageFullWidth, int ImageFullHeight, long HeaderSize);
double  *readFitsImageBlockD(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int ImageFullWidth, int ImageFullHeight, long HeaderSize);
int     *readFitsImageBlockI32(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int ImageFullWidth, int ImageFullHeight, long HeaderSize);
//double*takeFitsImagePixel();
double   takePixel(double *data, int ImageCoordX, int ImageCoordY, int ImageWidth, int ImageHeight);
int      isLittleEndian2();
char    *inverseBytes(char *content, int length);

/*readFitsBintable*/
int   readFitsXTENSION(const char *FilePath, const char* EXTNAME);
//readFitsXTENSION根据输入的BINTABLE的EXTENSION的名称EXTNAME和FITS文件来返回该EXTENSION的序号, 从0开始. 若失败则返回负数.
void *readFitsBintable(const char *FilePath, int XTENSION, int *DataXSize, int *DataYSize);
void *readFitsBintable(const char *FilePath, const char* EXTNAME, int *DataXSize, int *DataYSize);
int   readFitsBintable(const char *FilePath, int xtension, void **Data, char **UnitFormat, int *UnitSize, int *Length);
//int   readFitsImage(const char *chrFilePath, int xtension, char *ImageData, int *Width, int *Height);

/*readFitsSpectrum*/
int     readFitsSpectrumRows(const char *FilePath, const char *EXTNAME);
double *readFitsSpectrum(const char *FilePath, const char* EXTNAME);

/*writeFits*/
int writeFitsF(float *data, char *header, const char *strFilePath);
int writeFitsD(double *data, char *header, const char *strFilePath);
int writeFitsFS(float *data, int DataWidth, int DataHeight, const char *strFilePath);
int writeFitsDS(double *data, int DataWidth, int DataHeight, const char *strFilePath);
int checkHead2880(long headBytes, FILE* writeToFile = NULL);
int checkData2880(long dataBytes, FILE* writeToFile = NULL);

char *generateFitsHeaderFS(int DataWidth, int DataHeight);
char *generateFitsHeaderDS(int DataWidth, int DataHeight);

/*modifyFitsImage*/
int modifyFitsImage(double *data, int DataWidth, int DataHeight, int InsertPosI0, int InsertPosJ0, const char *strFilePath, int xtension = 0, int debugcode = 0);


#endif // MYFITSIO_H
