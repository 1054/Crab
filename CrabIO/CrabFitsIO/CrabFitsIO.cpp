/* CrabFits Updated 2012-09-16 */
/* CrabFits Updated 2017-08-03 Ziegelhausen */
//
//Usage:
//    char  *header;
//    float *image;
//    int    flag;
//    header = readFitsHeader("c:\\liudz\\1.fits")
//    image  = readFitsImageF("c:\\liudz\\1.fits", 998, 1006, strlen(header));
//    flag   = writeFitsFS(image,998,1006,"随便的名字.fits");
//
#include "CrabFitsIO.h"


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*read standard fits header reader.*/
//standard fits header contains these lines:
// 1. keyword line (number value) (baLine[8]=="=")
// 2. keyword line (string value) (baLine[8]=="=")
// 3. comment line                (baLine[0:6]=="COMMENT")
// 4. history line                (baLine[0:6]=="HISTORY")
// 5. end of header line
char *readFitsHeader(const char *FilePath)
{
    char *HeaderText=NULL;
    long hPosition=0;
    long hSize=0;
    readFitsHeader(FilePath, 0, &HeaderText, &hPosition, &hSize);
    return HeaderText;
}

char *readFitsHeader(const char *FilePath, int xtension)
{
    char *HeaderText=NULL;
    long hPosition=0;
    long hSize=0;
    readFitsHeader(FilePath, xtension, &HeaderText, &hPosition, &hSize);
    return HeaderText;
}

int readFitsHeader(const char *FilePath, char **HeaderText)
{
    long hPosition=0;
    long hSize=0;
    return readFitsHeader(FilePath, 0, HeaderText, &hPosition, &hSize);
}

int readFitsHeader(const char *FilePath, int xtension, char **HeaderText, long *HeaderPosition, long *HeaderSize)
{
    int debugflag=0; //set to 1 to debug
    int errorcode=0;
    int isHeader=0;    //whether this line is simple text (header).
    int isEnded=0;     //whether this header section has ended.
    long currentLine=0; //current line number when dealing with the header.
    long currentPosition=0; //current position
    long xtensionSection=0; //extension contents' count.
    long xtensionPosition=0; //extension contents' first byte position.
    long xtensionSize=0; //extension contents' size.
    long headerSizeCheck=0; //the output header's size.
    long fileSize=0;
    char *baLine=NULL;     //byte array containing a line.
    /*check input FilePath*/
    if(NULL==FilePath) { printf("readFitsHeader: Error! Input FilePath is NULL!\n"); errorcode=-1; return errorcode; } //<added><20141031><dzliu>
    /*Open the fits file.*/
    FILE *fp;
    fp = fopen(FilePath,"rb");
    if(fp==NULL)
    {
        printf("readFitsHeader: Error! Fits file %s can not be opened!\n",FilePath);
        errorcode = -5;//file not exist. cannot open it.
        return errorcode;
    }
    /*Calculate the file size.*/
    fseek(fp,0,SEEK_END);
    fileSize=ftell(fp);
    /*Rewind the file pointer and initial parameters.*/
    fseek(fp,0,SEEK_SET);
    isHeader=1; isEnded=0; currentLine=0; currentPosition=0;
    baLine = (char *)malloc(81*sizeof(char));
    memset(baLine,0x20,80);
    *(baLine+80)='\0';
    /*Begin loop.*/
    while(!feof(fp))
    {
        /*Read a line.*/
        currentPosition += fread(baLine,1,80,fp);
        currentLine++;
        /*Examine the XTENSION and END mark.*/
        if(isXTENSION(baLine)) //if it's XTENSION mark, then start a new section.
        {
            xtensionSection++;
            if(xtension>=0 && xtensionSection>xtension)
                break;
            xtensionPosition=currentPosition-80;
            isEnded=0;
            if(debugflag>0) {
                printf("readFitsHeader: debug: found extension %ld at position %ld.\n", xtensionSection, xtensionPosition);
            }
        }
        else if(isENDmark(baLine)) //if it's END mark, then close the section.
        {
            isEnded=1;
        }
        ////safe mode, in case there is no END mark.
        //else if(isHeader && !isSimpleText(baLine)) //previous is header, but current is data
        //{
        //    isEnded=1; //this will lead to "else...//after END mark, it's data."
        //}
        /*Examine the line.*/
        if((xtension>=0 && xtensionSection==xtension) || (xtension==-1))
        {
            if(!isEnded) //before END mark.
            {
                isHeader=1;
                xtensionSize+=80;
                if(debugflag>0) {
                    printf("readFitsHeader: debug: reading extension %ld at position %ld.\n", xtensionSection, xtensionPosition+xtensionSize);
                }
            }
            else         //after END mark.
            {
                if(isBlankSpace(baLine)) //after END mark, but it's blankspace.
                {
                    isHeader=1;
                    xtensionSize+=80;
                }
                else if(isENDmark(baLine))
                {
                    isHeader=1;
                    xtensionSize+=80;
                }
                else                     //after END mark, it's data.
                {
                    isHeader=0;
                    break;
                }
            }
        }
    }
    /*Malloc array only when xtensionSize>0 (20140826DL)*/
    if(xtensionSize>0) {
        *HeaderSize = xtensionSize;
        *HeaderPosition = xtensionPosition;
        *HeaderText = (char *)malloc((xtensionSize+1)*sizeof(char)); //+1 means '\0'=0x00.
        //rewind(fp);
        fseek(fp,xtensionPosition,SEEK_SET);
        headerSizeCheck = fread(*HeaderText,1,xtensionSize,fp);
        (*HeaderText)[xtensionSize] = 0x00;
        //printf("readFitsHeader: Working well! Header of extension %i has been read!\n",xtension);
    } else {
        *HeaderText = NULL; *HeaderPosition=-1; *HeaderSize=0;
        printf("readFitsHeader: Error! No header found with extension %i!\n",xtension);
        errorcode = -1;
    }
    /*Clean obsolete arrays.*/
    free((void *)baLine);
    baLine=NULL;
    fclose(fp);
    return errorcode;
}

int readFitsHeaderExtensions(const char *FilePath)
{
    FILE *fp;
    int xtensionSection=0;
    /*Open the fits file.*/
    fp = fopen(FilePath,"rb");
    if(fp==NULL)
    {
        printf("readFitsHeaderExtensions: Error! Fits file %s can not be opened!\n",FilePath);
        return -1;
    }
    /*Rewind the file pointer and initial parameters.*/
    fseek(fp,0,SEEK_SET);
    char *baLine;
    baLine = (char *)malloc(81*sizeof(char));
    memset(baLine,0x20,80);
    *(baLine+80)='\0';
    /*Begin loop.*/
    while(!feof(fp))
    {
        /*Read a line.*/
        fread(baLine,1,80,fp);
        /*Examine the XTENSION mark.*/
        if(isXTENSION(baLine)) //if it's XTENSION mark, then start a new section.
        {
            xtensionSection++;
        }
    }
    /*Clean obsolete arrays.*/
    free((void *)baLine);
    baLine=NULL;
    fclose(fp);
    return xtensionSection;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*find and extract a keyword value.*/
char *extKeyword(const char *KeywordName, const char *HeaderText, int KeepQuotes, int debugcode)
{
    char *KeywordValue=NULL;
    extKeyword(KeywordName,HeaderText,&KeywordValue,KeepQuotes,debugcode);
    return KeywordValue;
}

int extKeyword(const char *KeywordName, const char *HeaderText, char **KeywordValue, int KeepQuotes, int debugcode)
{
    //20170807: added input argument "KeepQuotes". If KeepQuotes == 1 then keep all kinds of quotes. If KeepQuotes == 2 then keep only 0x22 QUOTATION MARK. If KeepQuotes == 3 then keep only 0x27 APOSTROPHE.
    int errorcode=0;
    int i=0,j=0;
    unsigned int k=0;
    int match=0;
    int findSlash=0;
    long HeaderSize=0;
    int HeaderLine=0;
    int KeySize=0;
    char *baLine;
    char *kValue;
    int isTrimmed=0;
    /*check input HeaderText*/
    if(NULL==HeaderText) { printf("extKeyword: Error! Input HeaderText is NULL!\n"); errorcode=-1; return errorcode; } //<added><20141031><dzliu>
    baLine = (char *)malloc(81*sizeof(char));
    kValue = (char *)malloc(72*sizeof(char));
    memset(baLine,0x0,81);
    memset(kValue,0x0,71);
    HeaderSize = strlen(HeaderText);
    HeaderLine = HeaderSize/80;
    for(j=0; j<HeaderLine; j++)
    {
        k=j*80;
        for(i=0;i<80;i++) //copy one header line to analyse
            baLine[i]=HeaderText[k+i];
        //Compare the keyword name.
        if(strlen(KeywordName)<8) KeySize=strlen(KeywordName);
        else KeySize=8;
        match = strncmp(baLine,KeywordName,KeySize);  //case sensitive! TODO:case.
        if(0==match) //find the keyword line, grab the value.
        {
            //find the slash "/" mark.
            findSlash = locateSlash(baLine,8,79); //it should be 31 or 32.
            if(findSlash<=0) //slash found, take the string before slash as value.
                findSlash=80; //if slash not found, take the rest whole line as value.
            //trim left blankspace.
            k=0; isTrimmed=0;
            for(i=9;i<findSlash;i++) //usually [8] is "=".
            {
                if(!isTrimmed && baLine[i]!=0x20)
                {
                    kValue[k++] = baLine[i];
                    isTrimmed=1;
                }
                else if(isTrimmed) //record the text after trimmed.
                {
                    kValue[k++] = baLine[i];
                }
            }
            //trim right blankspace.
            k = strlen(kValue)-1;
            while(0x20==kValue[k])
            {
                kValue[k--]=0x0;
            }
            //trim quote pair (0x27 APOSTROPHE) if exists.
            if(1!=KeepQuotes && 3!=KeepQuotes) {
                k = strlen(kValue)-1;
                if(0x27==kValue[0] && 0x27==kValue[k])
                {
                    kValue[k]=0x0; //this will make strlen(kValue) decrese 1.
                    k=0;
                    while(k < strlen(kValue)-1)
                    {
                        kValue[k]=kValue[k+1];
                        k++;
                    }
                    kValue[k]=0x0; //this time k == strlen(kValue)-1.
                }
            }
            //trim quote pair (0x22 QUOTATION MARK) if exists.
            if(1!=KeepQuotes && 2!=KeepQuotes) {
                k = strlen(kValue)-1;
                if(0x22==kValue[0] && 0x22==kValue[k])
                {
                    kValue[k]=0x0; //this will make strlen(kValue) decrese 1.
                    k=0;
                    while(k < strlen(kValue)-1)
                    {
                        kValue[k]=kValue[k+1];
                        k++;
                    }
                    kValue[k]=0x0; //this time k == strlen(kValue)-1.
                }
            }
            //trim blank space at begin
            while(0x20==kValue[0])
            {
                k = 0;
                while(k < strlen(kValue)-1)
                {
                    kValue[k]=kValue[k+1];
                    k++;
                }
                kValue[k]=0x0; //this time k == strlen(kValue)-1.
            }
            //trim blank space at end
            k = strlen(kValue)-1;
            while(0x20==kValue[k])
            {
                kValue[k--]=0x0;
            }
            //<TODO:> check kValue.
            free(baLine);
            baLine=NULL;
            if(0x0!=kValue[0])
                (*KeywordValue)=kValue;
            kValue=NULL;
            errorcode=0;
            return errorcode;
        }
    }
    //if keywordName not found.
    free(baLine);
    baLine=NULL;
    free(kValue);
    kValue=NULL;
    (*KeywordValue)=NULL;
    errorcode=-1;
    return errorcode;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*find and modify a keyword value.*/
int modKeyword(const char *strKeyName, const char *strKeyValue, char *strHeader)
{
    int errorcode=0;         //return 0 if found and modified the key, return -1 if key not found. 
    int isHeader=0;          //whether this line is simple text (header).
    int flagNumberKeyword=0; //whether this line is a number type keyword line.
    int flagStringKeyword=0; //whether this line is a string type keyword line.
    int flagKeyword=0; //whether this line is a keyword line.
    int flagComment=0; //whether this line is a comment line.
    int flagHistory=0; //whether this line is a history line.
    int flagENDmark=0; //whether this line is a END marked line.
    int findQuote1=0,findQuote2=0,findSlash=0; //find and locate the ' in a line.
    int countLines=0; //current line number when dealing with the header. count total header lines.
    int countSize =0; //current header size when dealing with the header. count total header size.
    char *strKeywordName;
    char *strKeywordValue;
    char *baLine;    //byte array containing a line.
    int i=0,j=0,k=0, headerSize=0, match=-1, valueSize=0, valueQuote1=10, valueQuote2=79, valueTail=0;
    char *newHeader=NULL;
    /*check input strHeader*/
    if(NULL==strHeader) { printf("modKeyword: Error! Input strHeader is NULL!\n"); return -1; } //<added><20141031><dzliu>
    /*initial parameters.*/
    headerSize=strlen(strHeader);
    isHeader=1; countLines=0; countSize=0;
    newHeader = (char *)strHeader; //newHeader = (char *)malloc(headerSize*sizeof(char));
    baLine = (char *)malloc(81*sizeof(char));
    strKeywordName = (char *)malloc(9*sizeof(char));
    //memset(newHeader,0x20,headerSize);
    memset(baLine,0x20,80);
    memset(strKeywordName,0x20,8);
    *(baLine+80)='\0';
    *(strKeywordName+8)=0x00;
    //Copy the keyword name.
    k=(int)strlen(strKeyName);
    for(i=0;i<8;i++)
    {
        if(i<k)
            *(strKeywordName+i)=*(strKeyName+i);
        else
            *(strKeywordName+i)=0x20; // the keyname is a 8-byte string, including white space.
    }
    //Copy and trim the new keyword value.
    strKeywordValue = strtrim2(strKeyValue);
    valueSize = strlen(strKeywordValue);
    if(valueSize>71) valueSize=71; //check the input outflow. the input value size should be .LE. 71.
    //printf("strlen of the keyvalue is %d \n",valueSize);
    /*Begin loop.*/
    errorcode=-1;
    while(isHeader==1)
    {
        //read line by line
        for(i=0;i<80;i++)
            *(baLine+i) = *(newHeader+countSize+i);
        countSize+=80; countLines++;
        //Classify this line
        flagKeyword=0;flagENDmark=0;flagComment=0;flagHistory=0;
        if(!isSimpleText(baLine))     isHeader=0;
        else if(*(baLine+8)==0x3D) flagKeyword=1; //"=" ASCII is 0x3D.
        else if(isENDmark(baLine)) flagENDmark=1;
        else if(isComment(baLine)) flagComment=1;
        else if(isHistory(baLine)) flagHistory=1;
        //deal with different type of lines
        if(flagKeyword==1)
        {
            /*Compare the keyword name.*/
            match = strncmp(baLine,strKeywordName,8);
            findSlash = locateSlash(baLine,10,79);
            if(match==0)
            {
                //string type? number type?
                findQuote1=locateQuote(baLine,9,79);
                //string type. (contains a pair of quote.)
                if(findQuote1>=9 && findQuote1<=11) //TODO: test the position of ' . Is it always 10?
                {
                    flagStringKeyword=1;
                    findQuote2=locateQuote(baLine,findQuote1+1,79);
                    findSlash =locateSlash(baLine,findQuote2+1,79); //if not found, it is -1.
                    //new value string begins @valueQuote1 and ends @valueQuote2, including the pair of ' '.
                    valueQuote1=findQuote1;
                    valueQuote2=findQuote1+valueSize+1;
                    //AABBCCDD= '1234566777889' / xxxxxxxxx
                    //          '------------>' / xxxxxxxxx (recording range.)
                    //AABBCCDD= '1234566777889000000019293'
                    //          '------------------------>' (recording range.)
                    //Decide where to write the pair of quotes.
                    if(findSlash>findQuote2 && valueQuote2>(findSlash-2))
                        valueQuote2=(findSlash-2);
                    else if(findSlash<=0 && valueQuote2>79)
                        valueQuote2=79;
                    //To write blank space outside the pair of quotes, we need to decide the outer range -
                    //of the KeyValue.
                    if(findSlash>findQuote2)
                        valueTail=findSlash;
                    else
                        valueTail=80;
                    //Write down the pair of quotes.
                    *(newHeader+countSize-80+valueQuote1) = 0x27; //"'"
                    *(newHeader+countSize-80+valueQuote2) = 0x27; //"'"
                    //Write chars in the pair of quotes, and blank spaces outside the quotes.
                    for(i=valueQuote1+1,j=0 ;i<valueTail; i++,j++)
                    {
                        if(i<valueQuote2)
                            *(newHeader+countSize-80+i) = *(strKeywordValue+j);
                        else if(i>valueQuote2)
                            *(newHeader+countSize-80+i) = 0x20;
                    }
                }
                //number type. (contains no quote.)
                else
                {
                    flagNumberKeyword=1;
                    findSlash=locateSlash(baLine,9,79); //it should be 31 or 32.
                    if(findSlash<=0) findSlash=31;
                    for(i=(findSlash-2),j=(valueSize-1); i>=9; i--,j--) //i=8 is the '=' mark.
                    {   //write new value into header.
                        //AABBCCDD=      0.555 / xxxxxxxx
                        //AABBCCDD=    123.555 /
                        //          <--------- (recording range.)
                        if(j>=0)
                            *(newHeader+countSize-80+i) = *(strKeywordValue+j);
                        else
                            *(newHeader+countSize-80+i) = 0x20;
                    }
                }
                errorcode=0;
                break;
            }
        }
    }
    /*Clean mem.*/
    free((void *)(baLine));
    free((void *)(strKeywordName));
    //free((void *)(strKeywordValue));
    baLine=NULL;
    strKeywordName=NULL;
    strKeywordValue=NULL;
    return errorcode;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*add a keyword value.*/
int addKeyword(const char *strKeyName, const char *strKeyValue, char **HeaderPointer, const char *strComment, int addEmptyLine, int debugcode)
{
    //int debugcode=0;
    int errorcode=0;
    int i=0,k=0;
    int isHeader=0;          //whether this line is simple text (header).
    int countLines=0;
    int countBytes=0;
    int isBlank=0,flagENDmark=0;
    char *strHeader = (*HeaderPointer);
    long oldHeaderSize=strlen(strHeader); // should be integer times of 36*80=2880
    long oldHeaderValidLine=0;
    long oldHeaderEmptyLine=0;
    long oldHeaderEndedLine=0;
    long  newHeaderSize = 0;
    long  newHeaderInsertLine = 0;
    long  newHeaderEndingLine = 0;
    long  newHeaderAddedBytes = 0;
    char *newHeader = NULL;
    char *newHeaderPtr = NULL;
    char *strKeywordName = NULL;
    char *strKeywordValue = NULL;
    char *strKeywordComment = NULL;
    int   intKeywordValueBytes;
    int   intKeywordCommentBytes;
    char *baLine = NULL; //byte array containing a line.
    baLine = (char *)malloc((80+1)*sizeof(char)); //Prepare input header line container
    if(debugcode>0) {printf("addKeyword: begin\n");}
    /*Check whether keyword exists.*/
    errorcode = modKeyword(strKeyName,strKeyValue,strHeader);
    if(errorcode==-1) {
        errorcode=0; // <20170920> errorcode=-1;
        if(debugcode>0) {printf("addKeyword: No existing keyword, adding a new one.\n");}
        // new keyword! Now start to add the keyword after the last header text line.
        //
        /*Begin loop to read header text.*/
        isHeader=1;
        while(isHeader==1) {
            //read line by line
            memset(baLine,0x20,80);
            memset(baLine+80,0x00,1);
            for(i=0;i<80;i++) {
                *(baLine+i) = *(strHeader+countBytes+i);
            }
            //Classify this line
            isBlank=0; flagENDmark=0; // isBlank -- whether this line is blank
            if(!isSimpleText(baLine))     isHeader=0;
            else if(isBlankSpace(baLine)) isBlank=1;
            else if(isENDmark(baLine)) flagENDmark=1;
            if(debugcode>1) {printf("addKeyword: header line %04d: %s\n",countBytes/80+1,baLine);} // <20170920><debug>
            //Find the last blank line
            // <20170920><buggy> if(1==isHeader && 0==isBlank && 0==flagENDmark) { oldHeaderValidLine = countBytes/80+1; }
            // <20170920><buggy> if(1==isHeader && 1==isBlank && 0==flagENDmark) { oldHeaderEmptyLine = countBytes/80+1; }
            if(1==isHeader && 0==isBlank && 0==flagENDmark) { oldHeaderValidLine = countBytes/80+1; oldHeaderEmptyLine=-1; } // <20170920>
            if(1==isHeader && 1==isBlank && 0==flagENDmark && oldHeaderEmptyLine==-1) { oldHeaderEmptyLine = countBytes/80+1; } // <20170920>
            //Untill we touch the last header line with END mark
            if(1==flagENDmark) {
                //Record current old header END position
                oldHeaderEndedLine = countBytes/80+1; // <20170920> oldHeaderEndedLine = countBytes/80;
                if(debugcode>0) {printf("addKeyword: oldHeaderValidLine = %ld\n",oldHeaderValidLine);}
                if(debugcode>0) {printf("addKeyword: oldHeaderEmptyLine = %ld\n",oldHeaderEmptyLine);}
                if(debugcode>0) {printf("addKeyword: oldHeaderEndedLine = %ld\n",oldHeaderEndedLine);}
                if(debugcode>0) {printf("addKeyword: addEmptyLine = %d\n",addEmptyLine);}
                //Insert the new keyword line two lines after the oldHeaderValidLine.
                newHeaderInsertLine = oldHeaderValidLine + addEmptyLine + 1;
                if(debugcode>0) {printf("addKeyword: Will insert at the %ld-th line.\n",newHeaderInsertLine);}
                //If the insert line is after the END line, then add one new header block of 2880 bytes.
                if(newHeaderInsertLine>=oldHeaderEndedLine) {
                    if(debugcode>0) {printf("addKeyword: Will add a new header block.\n");}
                    newHeaderSize = (long(oldHeaderSize/2880)+1) * 2880; // Add one new header block, i.e. 36 lines (36*80bytes=2880bytes)
                    newHeaderEndingLine = newHeaderSize / 80; // <buggy><20170920> (long(oldHeaderEndedLine/36)+1) * 36;
                    if(debugcode>0) {printf("addKeyword: The new header will have %ld bytes (the old header has %ld bytes).\n",newHeaderSize,oldHeaderSize);}
                    if(debugcode>0) {printf("addKeyword: The new header will have %ld lines (the old header has %ld lines).\n",newHeaderEndingLine,oldHeaderEndedLine);}
                } else {
                    newHeaderSize = (long(oldHeaderSize/2880)) * 2880;
                    newHeaderEndingLine = (long(oldHeaderEndedLine/36)) * 36;
                    if(debugcode>0) {printf("addKeyword: The new header will have %ld bytes (the old header has %ld bytes).\n",newHeaderSize,oldHeaderSize);}
                    if(debugcode>0) {printf("addKeyword: The new header will have %ld lines (same as the old header).\n",newHeaderEndingLine);}
                }
                //
                long lonByteAtEnd = countBytes; //TODO:delete
                //Prepare output new header buffer
                newHeader = (char *)malloc((newHeaderSize+1)*sizeof(char));
                memset(newHeader,0x20,newHeaderSize);
                memset(newHeader+newHeaderSize,0x00,1);
                //printf("\ndebug:oldHeaderSize %ld, newHeaderSize %ld\n",oldHeaderSize,newHeaderSize);
                //Prepare keyword name buffer
                strKeywordName = (char *)malloc((8+1)*sizeof(char));
                memset(strKeywordName,0x20,8);
                memset(strKeywordName+8,0x00,1);
                //Copy the keyword name.
                k=(int)strlen(strKeyName);
                for(i=0;i<8;i++){if(i<k){*(strKeywordName+i)=*(strKeyName+i);}else{*(strKeywordName+i)=0x20;}} // the keyname is a 8-byte string, including white space.
                //Copy and trim the new keyword value.
                strKeywordValue = strtrim2(strKeyValue);
                intKeywordValueBytes = strlen(strKeywordValue);
                if(debugcode>0) {printf("addKeyword: strlen of the input key value is %d bytes!\n",intKeywordValueBytes);}
                if(intKeywordValueBytes>71) {
                    intKeywordValueBytes=71; //check the input outflow. the input value size should be .LE. 71.
                    if(debugcode>0) {printf("addKeyword: Warning! Trimming the input key value to 71 bytes!\n");}
                }
                //Copy and trim the new keyword comment.
                if(strComment) {
                    strKeywordComment = strtrim2(strComment);
                    intKeywordCommentBytes = strlen(strKeywordComment);
                    if(intKeywordValueBytes+intKeywordCommentBytes>71-3) intKeywordCommentBytes=71-3-intKeywordValueBytes; //check the input outflow. if value is too long, then there will be no place for intKeywordCommentBytes. Note that there has a 3 bytes string " / " between key value and key comment.
                    if(debugcode>0) {printf("addKeyword: strlen of the input key comment is %d bytes.\n",intKeywordCommentBytes);}
                } else {
                    intKeywordCommentBytes = 0;
                }
                //Copy the header from old header buffer to new header buffer, for only valid header lines, i.e. not empty lines
                newHeaderPtr = newHeader;
                strncpy(newHeaderPtr,strHeader,oldHeaderValidLine*80);
                newHeaderPtr+=(oldHeaderValidLine*80);
                if(debugcode>0) {printf("addKeyword: copied %ld lines from old header %p to new header %p.\n",oldHeaderValidLine,&strHeader,&newHeader);}
                //Adding two empty lines
                newHeaderPtr+=(addEmptyLine*80);
                if(debugcode>0) {printf("addKeyword: skiped %d lines as separation.\n",addEmptyLine);}
                //Insert new keyword line, 80bytes, and append the rest of old header contents.
                int intByteAdded = 0; // adedd how many keyword bytes, one line is 80bytes
                strncpy(newHeaderPtr,strKeywordName,8);
                newHeaderPtr+=8;
                intByteAdded+=8;
                strncpy(newHeaderPtr,"= ",2);
                newHeaderPtr+=2;
                intByteAdded+=2;
                strncpy(newHeaderPtr,strKeywordValue,intKeywordValueBytes);
                newHeaderPtr+=intKeywordValueBytes;
                intByteAdded+=intKeywordValueBytes;
                if(intKeywordCommentBytes>0) {
                    strncpy(newHeaderPtr," / ",3);
                    newHeaderPtr+=3;
                    intByteAdded+=3;
                    strncpy(newHeaderPtr,strKeywordComment,intKeywordCommentBytes);
                    newHeaderPtr+=intKeywordCommentBytes;
                    intByteAdded+=intKeywordCommentBytes;
                }
                while(intByteAdded%80!=0) {
                    strncpy(newHeaderPtr," ",1);
                    newHeaderPtr+=1;
                    intByteAdded+=1;
                }
                newHeaderAddedBytes = intByteAdded;
                if(debugcode>0) {printf("addKeyword: added %ld bytes as the new keyword-value line at the %ld-th line of the new header.\n",newHeaderAddedBytes,newHeaderInsertLine);}
                //Copy the last lines
                //if(0) {
                //printf("\ndebug:newHeader+%ld oldHeader+%ld size=%ld\n",
                //                  newHeaderSize-(oldHeaderSize-lonByteAtEnd),
                //                                               lonByteAtEnd,
                //                                 oldHeaderSize-lonByteAtEnd);
                //}
                //strncpy(newHeader+newHeaderSize-(oldHeaderSize-lonByteAtEnd),
                //                                     strHeader+lonByteAtEnd,
                //                                 oldHeaderSize-lonByteAtEnd);
                newHeaderPtr = newHeader + (newHeaderEndingLine-1)*80; // <20170920> newHeaderPtr = newHeader + newHeaderEndingLine*80;
                memset(newHeaderPtr,0x20,80);
                memset(newHeaderPtr+80,0x00,1);
                strncpy(newHeaderPtr,"END",3);
                if(debugcode>0) {printf("addKeyword: the new header ends at the %ld-th line.\n",newHeaderEndingLine);}
                //<OLD><BEFORE><20160704>// newHeaderPtr+=intByteToAdd;
                //<OLD><BEFORE><20160704>// strncpy(newHeaderPtr,strHeader+oldHeaderValidSize,oldHeaderSize-oldHeaderValidSize);
                // printf("\ndebug:strlen(newHeader)=%d\n",strlen(newHeader));
                // printf("\ndebug:countBytes=%d,oldHeaderValidSize=%d\n%s\n",countBytes,oldHeaderValidSize,newHeader);
                isHeader=0; errorcode=0;
                break;
            }
            //<DONE>
            //<DONE> WRITE A CODE TO ADD KEYWORD !!! <20150215>
            //<DONE>
            //next
            countBytes+=80; countLines++;
        }
        //if(0){
        //    printf("debug:strKeywordName = %s\n", strKeywordName);
        //    printf("debug:strKeywordValue = %s\n", strKeywordValue);
        //    printf("debug:oldHeaderSize=%ld\n",strlen(strHeader));
        //    printf("debug:newHeaderSize=%ld\n",strlen(newHeader));
        //}
        if(baLine) { free(baLine); baLine=NULL; }
        if(strKeywordName) { free(strKeywordName); strKeywordName=NULL; }
        // printf("\ndebug:strHeader=%d\n",strHeader);
        // printf("\ndebug:newHeader=%d\n",newHeader);
        // printf("\ndebug:\n%s\n",strHeader);
        // printf("\ndebug:\n%s\n",newHeader);
        // if(strHeader) { printf("debug freeing old header %d (strlen=%d)\n", strHeader, strlen(strHeader)); free(strHeader); }
        if(newHeader) {
            if(debugcode>0) {printf("addKeyword: old header is %p.\n",strHeader);}
            if(strHeader) { free(strHeader); strHeader=NULL; }
            if(debugcode>0) {printf("addKeyword: new header is %p.\n",newHeader);}
            *HeaderPointer = newHeader;
            // (*HeaderText)=newHeader; newHeaderPtr=NULL; newHeader=NULL;
            // strHeader = (char *)realloc(strHeader,(strlen(newHeader)+1)*sizeof(char));
            // memset(strHeader,0x0,(strlen(newHeader)+1));
            // strncpy(strHeader,newHeader,newHeaderSize);
            // printf("debug:newHeader=0x%x\n",(*HeaderText));
            // newHeaderPtr=NULL; free(newHeader); newHeader=NULL;
            //// printf("debug:strHeader=0x%x\n",strHeader);
            //// printf("debug:newHeader=0x%x\n",newHeader);
        }
    }
    if(debugcode>0) {printf("addKeyword: done with errorcode %d (0=ok)\n",errorcode);}
    return errorcode;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*add a comment keyword line.*/
int addComment(const char *strComment, char *strHeader)
{
    int errorcode=0;
    int isHeader=0;    //whether this line is simple text (header).
    int flagKeyword=0; //whether this line is a keyword line.
    int flagComment=0; //whether this line is a comment line.
    int flagHistory=0; //whether this line is a history line.
    int flagENDmark=0; //whether this line is a END marked line.
    int findLastNonblank=0;//find and locate the last nonblank line.
    int findLastComment=0;//find and locate the last comment line.
    int findENDmark=0;    //find and locate the END marked line.
    int countLines=0; //current line number when dealing with the header. count total header lines.
    int countSize=0; //current header size when dealing with the header. count total header size.
    char *baLine;    //byte array containing a line.
    int i=0,j=0,j1=0,j2=0,i8=0,k=0, headerSize=0;
    int newCommentLines=0, newCommentSize=0, newHeaderSize=0, newBlankLines=0;
    char *newHeader=NULL;
    /*check input strHeader*/
    if(NULL==strHeader) { printf("addComment: Error! Input strHeader is NULL!\n"); return -1; } //<added><20141031><dzliu>
    /*initial parameters.*/
    headerSize=strlen(strHeader);
    isHeader=1; countLines=0; countSize=0;
    baLine = (char *)malloc(81*sizeof(char));
    memset(baLine,0x20,80);
    *(baLine+80)='\0';
    /*Begin loop.*/
    while(isHeader==1)
    {
        /*Read in one line.*/
        for(i=0;i<80;i++)
            *(baLine+i) = *(strHeader+countSize+i);
        countSize += 80; countLines++;
        /*Classify this line.*/
        flagKeyword=0;flagENDmark=0;flagComment=0;flagHistory=0;
        if(!isSimpleText(baLine))     isHeader=0;
        else if(*(baLine+8)==0x3D) flagKeyword=1; //"=" ASCII is 0x3D.
        else if(isENDmark(baLine)) flagENDmark=1;
        else if(isComment(baLine)) flagComment=1;
        else if(isHistory(baLine)) flagHistory=1;
        /*deal with different type lines.*/
        if(isHeader==0)
        {
            /*header invalid!*/
            errorcode=3;
        }
        else if(flagKeyword==1)
        {
            findLastNonblank=countLines-1;
        }
        else if(flagComment==1)
        {
            findLastComment=countLines-1; //starts from 0. 0 means line 1.
            findLastNonblank=countLines-1;
        }
        else if(flagHistory==1)
        {
            findLastNonblank=countLines-1;
        }
        else if(flagENDmark==1)
        {
            findENDmark=countLines-1;
        }
    }
    /*If no comment line found, than use the last line.*/
    if(findLastComment<=1)
        findLastComment=findLastNonblank;
    /*Insert new comment lines.*/
    newCommentSize = strlen(strComment);
    newCommentLines = newCommentSize/72;        //80-8=72
    if(newCommentSize%72!=0) newCommentLines+=1;
    //Fill more blank spaces to make sure the header lines are the integer times of 36.
    //According to IRAF, header size should be the integer times of 2880=36*80.
    if((findENDmark-findLastNonblank-1) >= (newCommentLines%36))
        newBlankLines = -newCommentLines%36; //there are enough blank lines to be removed.
    else                                //there is no enough blank lines, thus we must add more blank lines.
        newBlankLines = 36 - newCommentLines%36;
    //calculate the newheadersize.
    newHeaderSize = headerSize+newCommentLines*80+newBlankLines*80;
    newHeader = (char *)malloc((newHeaderSize+1)*sizeof(char));
    memset(newHeader,0x20,newHeaderSize);
    *(newHeader+newHeaderSize)=0x00;
    j1 = (findLastComment+1)*80; //the total bytes from 0 to the end of last comment line.
    for(i=0; i<j1; i++)
    {
        *(newHeader+i) = *(strHeader+i);
    }
    j2 = j1 + newCommentLines*80; //the total bytes from 0 to the end of new comment line.
    //insert the new comment after the old lastest comment line. The new comment may occupy multiple lines.
    j=0;
    for(k=0;k<newCommentLines;k++)  //insert new comment line by line.
    {
        i8=j1+k*80;
        *(newHeader+i8+0)='C';*(newHeader+i8+1)='O';*(newHeader+i8+2)='M';*(newHeader+i8+3)='M';
        *(newHeader+i8+4)='E';*(newHeader+i8+5)='N';*(newHeader+i8+6)='T';*(newHeader+i8+7)=' ';
        for(i=8; i<80; i++)
        {
            if(j<newCommentSize)
                *(newHeader+i8+i) = *(strComment+j);
            else
                *(newHeader+i8+i) = 0x20;
            j++;
        }
    }//after loop, i = (j1+(newCommentLines-1)*80+80) == j2. Thus next part will begin from i=j2.
    //copy the remaining part before the END mark.
    for(i=j2,j=j1; j<findENDmark*80; i++,j++)
    {
        *(newHeader+i) = *(strHeader+j);
    }
    j1=j; //j1=findENDmark;
    j2=i; //j2=(findLastComment+1)*80 + newCommentLines*80 + (findENDmark-findLastComment-1)*80;
    //Leave more blank spaces, or remove some blank lines,
    //           to make the header size to be integer times of 2880.
    j2 += newBlankLines*80;
    //copy the last 2880 bytes END part.
    for(i=j2,j=j1; j<headerSize; i++,j++)
    {
        *(newHeader+i) = *(strHeader+j);
    }

    /*Clean mem.*/
    free((void *)strHeader);
    free((void *)(baLine));
    strHeader=newHeader;
    baLine=NULL;
    newHeader=NULL;
    errorcode = 0;
    return 0;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*add a history keyword line.*/
int addHistory(const char *strHistory, char *strHeader)
{
    int errorcode=0;
    int isHeader=0;    //whether this line is simple text (header).
    int flagKeyword=0; //whether this line is a keyword line.
    int flagComment=0; //whether this line is a comment line.
    int flagHistory=0; //whether this line is a history line.
    int flagENDmark=0; //whether this line is a END marked line.
    int findLastNonblank=0;//find and locate the last nonblank line.
    int findLastHistory=0;//find and locate the last history line.
    int findENDmark=0;    //find and locate the END marked line.
    int countLines=0; //current line number when dealing with the header. count total header lines.
    int countSize =0; //current header size when dealing with the header. count total header size.
    char *baLine;    //byte array containing a line.
    int i=0,j=0,j1=0,j2=0,i8=0,k=0, headerSize=0;
    int newHistoryLines=0, newHistorySize=0, newHeaderSize=0, newBlankLines=0;
    char *newHeader=NULL;
    /*check input strHeader*/
    if(NULL==strHeader) { printf("addHistory: Error! Input strHeader is NULL!\n"); return -1; } //<added><20141031><dzliu>
    /*initial parameters.*/
    headerSize=strlen(strHeader);
    isHeader=1; countLines=0; countSize=0;
    baLine = (char *)malloc(81*sizeof(char));
    memset(baLine,0x20,80);
    *(baLine+80)='\0';
    /*Begin loop.*/
    while(isHeader==1)
    {
        /*Read in one line.*/
        for(i=0;i<80;i++)
        {
            *(baLine+i) = *(strHeader+countSize+i);
        }
        countSize += 80;
        countLines++;
        /*Classify this line.*/
        flagKeyword=0;flagENDmark=0;flagComment=0;flagHistory=0;
        if(!isSimpleText(baLine))     isHeader=0;
        else if(*(baLine+8)==0x3D) flagKeyword=1; //"=" ASCII is 0x3D.
        else if(isENDmark(baLine)) flagENDmark=1;
        else if(isComment(baLine)) flagComment=1;
        else if(isHistory(baLine)) flagHistory=1;
        /*deal with different type lines.*/
        if(isHeader==0)
        {
            /*header invalid!*/
            errorcode=3;
        }
        else if(flagKeyword==1)
        {
            findLastNonblank=countLines-1;
        }
        else if(flagComment==1)
        {
            findLastNonblank=countLines-1;
        }
        else if(flagHistory==1)
        {
            findLastHistory=countLines-1; //starts from 0. 0 means line 1.
            findLastNonblank=countLines-1;
        }
        else if(flagENDmark==1)
        {
            findENDmark=countLines-1;
        }
    }
    /*If no history line found, than use the last line.*/
    if(findLastHistory<=1)
        findLastHistory=findLastNonblank;
    /*Insert new History lines.*/
    newHistorySize = strlen(strHistory);
    newHistoryLines = newHistorySize/72;        //80-8=72
    if(newHistorySize%72!=0) newHistoryLines+=1;
    //Fill more blank spaces to make sure the header lines are the integer times of 36.
    //According to IRAF, header size should be the integer times of 2880=36*80.
    if((findENDmark-findLastNonblank-1) >= (newHistoryLines%36))
        newBlankLines = -newHistoryLines%36; //there are enough blank lines to be removed.
    else                                //there is no enough blank lines, thus we must add more blank lines.
        newBlankLines = 36 - newHistoryLines%36;
    //calculate the newheadersize.
    newHeaderSize = headerSize+newHistoryLines*80+newBlankLines*80;
    newHeader = (char *)malloc((newHeaderSize+1)*sizeof(char));
    memset(newHeader,0x20,newHeaderSize);
    *(newHeader+newHeaderSize)='\0';
    j1 = (findLastHistory+1)*80; //the total bytes from 0 to the end of last History line.
    for(i=0; i<j1; i++)
        *(newHeader+i) = *(strHeader+i);
    j2 = j1 + newHistoryLines*80; //the total bytes from 0 to the end of new History line.
    //insert the new History after the old lastest History line. The new History may occupy multiple lines.
    j=0;
    for(k=0;k<newHistoryLines;k++)  //insert new History line by line.
    {
        i8=j1+k*80;
        *(newHeader+i8+0)='H';*(newHeader+i8+1)='I';*(newHeader+i8+2)='S';*(newHeader+i8+3)='T';
        *(newHeader+i8+4)='O';*(newHeader+i8+5)='R';*(newHeader+i8+6)='Y';*(newHeader+i8+7)=' ';
        for(i=8; i<80; i++)
        {
            if(j<newHistorySize)
                *(newHeader+i8+i) = *(strHistory+j);
            else
                *(newHeader+i8+i) = 0x20;
            j++;
        }
    }//after loop, i = (j1+(newHistoryLines-1)*80+80) == j2. Thus next part will begin from i=j2.
    //copy the remaining part before the END mark.
    for(i=j2,j=j1; j<findENDmark*80; i++,j++)
    {
        *(newHeader+i) = *(strHeader+j);
    }
    j1=j; //j1=findENDmark;
    j2=i; //j2=(findLastHistory+1)*80 + newHistoryLines*80 + (findENDmark-findLastHistory-1)*80;
    //Leave more blank spaces, or remove some blank lines,
    //           to make the header size to be integer times of 2880.
    j2 += newBlankLines*80;
    //copy the last 2880 bytes END part.
    for(i=j2,j=j1; j<headerSize; i++,j++)
    {
        *(newHeader+i) = *(strHeader+j);
    }

    /*Clean mem.*/
    free((void *)strHeader);
    free((void *)(baLine));
    //free((void *)(strKeywordName));
    //free((void *)(strKeywordValue));
    strHeader=newHeader;
    baLine=NULL;
    newHeader=NULL;
    //strKeywordName=NULL;
    //strKeywordValue=NULL;
    errorcode = 0;
    return 0;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isENDmark(char *baLine)
{
    int is=1;
    is=strncmp(baLine,"END     ",8);
    return !is;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isXTENSION(char *baLine)
{
    int is=1;
    is=strncmp(baLine,"XTENSION",8);
    return !is;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isComment(char *baLine)
{
    int is=1;
    is=strncmp(baLine,"COMMENT ",8);
    return !is;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isHistory(char *baLine)
{
    int is=1;
    is=strncmp(baLine,"HISTORY ",8);
    return !is;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isSimpleText(char *str8)
{
    int isText=1,nsize=0,i=0,j=0;
    char char8; //unsigned char? signed char?
    nsize = strlen(str8);
    isText=1;
    if(nsize==0) isText=0;
    for(i=0;i<nsize;i++)
    {
        char8=*(str8+i);
        j=(int)(char8);
        if(j<=31 || j>=127) isText=0; //<TODO>if we break after isText=0, it will be faster.
    }
    return isText;  //1 for yes, 0 for no.
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isSimpleText(char *str8, int length)
{
    int isText=1,nsize=0,i=0,j=0;
    char char8; //unsigned char? signed char?
    nsize = length;
    isText=1;
    if(nsize==0) isText=0;
    for(i=0;i<nsize;i++)
    {
        char8=*(str8+i);
        j=(int)(char8);
        if(j<=31 || j>=127) isText=0; //<TODO>if we break after isText=0, it will be faster.
    }
    return isText;  //1 for yes, 0 for no.
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isBlankSpace(char *str8)
{
    int isBlankSpace=1,nsize=0,i=0,j=0;
    char char8; //unsigned char? signed char?
    nsize = strlen(str8);
    isBlankSpace=1;
    if(nsize==0) isBlankSpace=0;
    for(i=0;i<nsize;i++)
    {
        char8=*(str8+i);
        j=(int)(char8);
        if(j!=32) isBlankSpace=0;
    }
    return isBlankSpace;  //1 for yes, 0 for no.
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int locateQuote(char *baLine, int nStart, int nEnd)
{   //find the first postion of the quote "'".
    int i=0,find=-1;
    i=nStart;
    if(nStart>nEnd) return -3;
    if(nStart<0 || nEnd>79) return -2;
    while(find==-1 && i<=nEnd)
    {
        if(*(baLine+i)==0x27) find=i; //"'" ASCII is 0x27.
        i++;
    }
    return find;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int locateSlash(char *baLine, int nStart, int nEnd)
{   //find the first postion of the slash "/". It should be 31.
    int i=0,find=-1;
    i=nStart;
    if(nStart>nEnd) return -3;
    if(nStart<0 || nEnd>79) return -2;
    while(find==-1 && i<=nEnd)
    {
        if(*(baLine+i)==0x2F) find=i; //"/" ASCII is 0x2F.
        i++;
    }
    return find;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
char *strtrim2(const char *str)
{
    char *head = (char *)str;
    char *tail = head + strlen(str); //tail now point to the end mark of a string '\0'.
    while(*head==' ')
    {
        head++;
    }
    tail--;
    while(*tail==' ')
    {
        *tail = '\0';
        tail--;
    }
    return head;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read FITS Image of different types.*/
double *readFitsImage(const char *chrFilePath, int xtension)
{
    /*Read fits header.*/
    long    HeaderPos  = 0;
    long    HeaderSize = 0;
    char   *HeaderData;
    int     errorcode = readFitsHeader(chrFilePath,xtension,&HeaderData,&HeaderPos,&HeaderSize);
    if (errorcode!=0) { printf("readFitsImage: Error! Could not get fits header from readFitsHeader!\n"); return NULL; }
    /*Read fits header keywords*/
    char   *BITPIX = extKeyword("BITPIX",HeaderData);
    char   *NAXIS1 = extKeyword("NAXIS1",HeaderData);
    char   *NAXIS2 = extKeyword("NAXIS2",HeaderData);
    int     ImageBitpix = atoi(BITPIX);
    int     ImageWidth  = atoi(NAXIS1);
    int     ImageHeight = atoi(NAXIS2);
    //printf("ImageBitpix=%d\n",ImageBitpix);
    //printf("ImageWidth=%d\n",ImageWidth);
    //printf("ImageHeight=%d\n",ImageHeight);
    if (ImageWidth<=0 || ImageHeight<=0) { printf("readFitsImage: Error! Could not get NAXIS from fits header!\n"); return NULL; }
    int     ImageSize = ImageWidth*ImageHeight;
    /*Read fits image data.*/
    if(ImageBitpix==32)
    {
        int *data2 = readFitsImageI32(chrFilePath, ImageWidth, ImageHeight, HeaderPos+HeaderSize);
        double *data1 = (double *)malloc(ImageSize*sizeof(double));
        for(int i=0; i<ImageSize; i++)
            data1[i] = double(data2[i]);
        return data1;
    }
    else if(ImageBitpix==-32)
    {
        float *data2 = readFitsImageF(chrFilePath, ImageWidth, ImageHeight, HeaderPos+HeaderSize);
        double *data1 = (double *)malloc(ImageSize*sizeof(double));
        for(int i=0; i<ImageSize; i++)
            data1[i] = double(data2[i]);
        return data1;
    }
    else if(ImageBitpix==-64)
    {
        return readFitsImageD(chrFilePath, ImageWidth, ImageHeight, HeaderPos+HeaderSize);
    }
    return NULL;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read float type (nbits=-32) FITS image.*/
float *readFitsImageF(const char *chrFilePath, int ImageWidth, int ImageHeight, long HeaderSize)
{
    float f4 = 0.0;
    char *f4p=NULL;//a char * pointer to f4.
    int   f4i = 0;
    int   nnX=0, nnY=0, nnN=0, nshift=0;
    int   i=0,j=0,m=0;
    float *dataArray4=NULL;
    FILE *fp;
    /*load parameters.*/
    nnX = ImageWidth;
    nnY = ImageHeight;
    nnN = nnX * nnY;
    dataArray4 = (float *)malloc(nnN*sizeof(float));
    if(dataArray4==NULL)
        return NULL; /*mem not enough.*/
    /*Open the file.*/
    fp = fopen(chrFilePath,"rb");
    if(fp==NULL)
        return NULL; /*"Cannot open the file for reading image data!"*/
    /*skip the header and prepare for reading image data.*/
    nshift = HeaderSize;
    fseek(fp,nshift,SEEK_SET);
    i=0;j=0;m=0l;
    /*read 4 bytes (32bits) float as a pixel value.*/
    for(m=0; m<nnN; m++)
    {
        if(isLittleEndian2()) //swap reading bytes order on Intel PC.
        {
            f4p = (char *)&f4;
            f4i = fread(&f4p[3], 1, 1, fp);
            f4i = fread(&f4p[2], 1, 1, fp);
            f4i = fread(&f4p[1], 1, 1, fp);
            f4i = fread(&f4p[0], 1, 1, fp);
        }
        else
        {
            f4i = fread(&f4, 4, 1, fp);
        }
        dataArray4[m]=f4;
    }
    fclose(fp);
    return dataArray4;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read double type (nbits=-64) FITS image.*/
double *readFitsImageD(const char *chrFilePath, int ImageWidth, int ImageHeight, long HeaderSize)
{
    double d4 = 0.0;
    char *d4p=NULL;//a char * pointer to f4.
    int   d4i = 0;
    int   nnX=0, nnY=0, nnN=0, nshift=0;
    int   i=0,j=0,m=0;
    double *dataArray4=NULL;
    FILE *fp;
    /*load parameters.*/
    nnX = ImageWidth;
    nnY = ImageHeight;
    nnN = nnX * nnY;
    dataArray4 = (double *)malloc(nnN*sizeof(double));
    if(dataArray4==NULL)
        return NULL;  //mem not enough.
    /*Open the file.*/
    fp = fopen(chrFilePath,"rb");
    if(fp==NULL)
        return NULL; //"Cannot open the file for reading image data!"
    /*skip the header and prepare for reading image data.*/
    nshift = HeaderSize;
    fseek(fp,nshift,SEEK_SET);
    i=0;j=0;m=0l;
    /*read 8 bytes (64bits) double as a pixel value.*/
    for(m=0; m<nnN; m++)
    {
        if(isLittleEndian2()) //swap reading bytes order on Intel PC.
        {
            d4p = (char *)&d4;
            d4i = fread(&d4p[7], 1, 1, fp);
            d4i = fread(&d4p[6], 1, 1, fp);
            d4i = fread(&d4p[5], 1, 1, fp);
            d4i = fread(&d4p[4], 1, 1, fp);
            d4i = fread(&d4p[3], 1, 1, fp);
            d4i = fread(&d4p[2], 1, 1, fp);
            d4i = fread(&d4p[1], 1, 1, fp);
            d4i = fread(&d4p[0], 1, 1, fp);
        }
        else
        {
            d4i = fread(&d4, 4, 1, fp);
        }
        dataArray4[m]=d4;
    }
    /*close the file.*/
    fclose(fp);
    return dataArray4;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read integer type (nbits=32) FITS image.*/
int *readFitsImageI32(const char *chrFilePath, int ImageWidth, int ImageHeight, long HeaderSize)
{
    int   i4 = 0;
    char *i4p=NULL;//a char * pointer to i4.
    int   i4i = 0;
    int   nnX=0, nnY=0, nnN=0, nshift=0;
    int   i=0,j=0,m=0;
    int *dataArray4=NULL;
    FILE *fp;
    /*load parameters.*/
    nnX = ImageWidth;
    nnY = ImageHeight;
    nnN = nnX * nnY;
    dataArray4 = (int *)malloc(nnN*sizeof(int));
    if(dataArray4==NULL)
        return NULL;  //mem not enough.
    /*Open the file.*/
    fp = fopen(chrFilePath,"rb");
    if(fp==NULL)
        return NULL; //"Cannot open the file for reading image data!"
    /*skip the header and prepare for reading image data.*/
    nshift = HeaderSize;
    fseek(fp,nshift,SEEK_SET);
    i=0;j=0;m=0l;
    /*read 4 bytes (32bits) integer as a pixel value.*/
    for(m=0; m<nnN; m++)
    {
        if(isLittleEndian2()) //swap reading bytes order on Intel PC.
        {
            i4p = (char *)&i4;
            i4i = fread(&i4p[3], 1, 1, fp);
            i4i = fread(&i4p[2], 1, 1, fp);
            i4i = fread(&i4p[1], 1, 1, fp);
            i4i = fread(&i4p[0], 1, 1, fp);
        }
        else
        {
            i4i = fread(&i4, 4, 1, fp);
        }
        dataArray4[m]=i4;
    }
    fclose(fp);
    return dataArray4;
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read FITS Image Block of different types.*/
double *readFitsImageBlock(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int xtension)
{
    /*Read fits header.*/
    long    HeaderPos  = 0;
    long    HeaderSize = 0;
    char   *HeaderData;
    int     errorcode = readFitsHeader(chrFilePath,xtension,&HeaderData,&HeaderPos,&HeaderSize);
    if (errorcode!=0) { printf("readFitsImageBlock: Error! Could not get fits header from readFitsHeader!\n"); return NULL; }
    /*Read fits header keywords*/
    char   *BITPIX = extKeyword("BITPIX",HeaderData);
    char   *NAXIS1 = extKeyword("NAXIS1",HeaderData);
    char   *NAXIS2 = extKeyword("NAXIS2",HeaderData);
    int     ImageBitpix = atoi(BITPIX);
    int     ImageFullWidth  = atoi(NAXIS1);
    int     ImageFullHeight = atoi(NAXIS2);
    //printf("ImageBitpix=%d\n",ImageBitpix);
    //printf("ImageWidth=%d\n",ImageWidth);
    //printf("ImageHeight=%d\n",ImageHeight);
    if (ImagePosI0<0 || ImagePosJ0<0 || ImagePosI0>=ImageFullWidth || ImagePosJ0>=ImageFullHeight) { printf("readFitsImageBlock: Error! Input ImagePosI0 or ImagePosJ0 is invalid!\n"); return NULL; }
    if (ImageWidth<=0 || ImageHeight<=0) { printf("readFitsImageBlock: Error! Input ImageWidth or ImageHeight is invalid!\n"); return NULL; }
    if (ImageFullWidth<=0 || ImageFullHeight<=0) { printf("readFitsImageBlock: Error! Could not get NAXIS from fits header!\n"); return NULL; }
    int ImageFullSize = ImageFullWidth*ImageHeight;
    int ImageSize = ImageWidth*ImageHeight;
    /*Free header data.*/
    free((void *)HeaderData); HeaderData=NULL;
    free((void *)BITPIX); BITPIX=NULL;
    free((void *)NAXIS1); NAXIS1=NULL;
    free((void *)NAXIS2); NAXIS2=NULL;
    /*Read fits image data.*/
    if(ImageBitpix==32)
    {
        int *data2 = readFitsImageBlockI32(chrFilePath, ImagePosI0, ImagePosJ0, ImageWidth, ImageHeight, ImageFullWidth, ImageFullHeight, HeaderPos+HeaderSize);
        double *data1 = (double *)malloc(ImageSize*sizeof(double));
        for(int i=0; i<ImageSize; i++)
            data1[i] = double(data2[i]);
        // free temporary data array
        free((void *)data2); data2=NULL;
        return data1;
    }
    else if(ImageBitpix==-32)
    {
        float *data2 = readFitsImageBlockF(chrFilePath, ImagePosI0, ImagePosJ0, ImageWidth, ImageHeight, ImageFullWidth, ImageFullHeight, HeaderPos+HeaderSize);
        double *data1 = (double *)malloc(ImageSize*sizeof(double));
        for(int i=0; i<ImageSize; i++)
            data1[i] = double(data2[i]);
        // free temporary data array
        free((void *)data2); data2=NULL;
        return data1;
    }
    else if(ImageBitpix==-64)
    {
        return readFitsImageBlockD(chrFilePath, ImagePosI0, ImagePosJ0, ImageWidth, ImageHeight, ImageFullWidth, ImageFullHeight, HeaderPos+HeaderSize);
    }
    return NULL;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read float type (nbits=-32) FITS image block.*/
float *readFitsImageBlockF(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int ImageFullWidth, int ImageFullHeight, long HeaderSize)
{
    float f4 = 0.0;
    char *f4p=NULL;//a char * pointer to f4.
    int   f4i = 0;
    int   nnX=0, nnY=0, nnN=0;
    long  nshift=0;
    int   i=0,j=0;
    long  m=0;
    float *dataArray4=NULL;
    FILE *fp;
    /*load parameters.*/
    nnX = ImageWidth;
    nnY = ImageHeight;
    nnN = nnX * nnY;
    dataArray4 = (float *)malloc(nnN*sizeof(float));
    if(dataArray4==NULL)
        return NULL; /*mem not enough.*/
    /*Open the file.*/
    fp = fopen(chrFilePath,"rb");
    if(fp==NULL)
        return NULL; /*"Cannot open the file for reading image data!"*/
    /*skip the header and prepare for reading image data.*/
    // we do not take entire image but take the image begining at ImagePosI0,ImagePosJ0
    // fseek(fp,nshift,SEEK_SET);
    i=0;j=0;m=0l;
    /*read 4 bytes (32bits) float as a pixel value.*/
    for(j=ImagePosJ0; j<ImagePosJ0+ImageHeight; j++)
    {
        nshift = HeaderSize+j*ImageFullWidth*sizeof(float)+ImagePosI0*sizeof(float); fseek(fp,nshift,SEEK_SET);
        for(i=ImagePosI0; i<ImagePosI0+ImageWidth; i++)
        {
            if(isLittleEndian2()) //swap reading bytes order on Intel PC.
            {
                f4p = (char *)&f4;
                for(long ibyte=sizeof(float)-1; ibyte>=0; ibyte--) {
                    f4i = fread(&f4p[ibyte], 1, 1, fp);
                }
                // printf("nshift=%ld f4=%lf \n",nshift,f4);
                // f4i = fread(&f4p[3], 1, 1, fp);
                // f4i = fread(&f4p[2], 1, 1, fp);
                // f4i = fread(&f4p[1], 1, 1, fp);
                // f4i = fread(&f4p[0], 1, 1, fp);
            }
            else
            {
                f4i = fread(&f4, 4, 1, fp);
            }
            dataArray4[m]=f4;
            m++;
        }
    }
    // printf("m=%ld!sizeof(float)=%ld!\n",m,sizeof(float));
    fclose(fp);
    return dataArray4;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read double type (nbits=-64) FITS image block.*/
double *readFitsImageBlockD(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int ImageFullWidth, int ImageFullHeight, long HeaderSize)
{
    double d4 = 0.0;
    char *d4p=NULL;//a char * pointer to f4.
    int   d4i = 0;
    int   nnX=0, nnY=0, nnN=0;
    long  nshift=0;
    int   i=0,j=0;
    long  m=0;
    double *dataArray4=NULL;
    FILE *fp;
    /*load parameters.*/
    nnX = ImageWidth;
    nnY = ImageHeight;
    nnN = nnX * nnY;
    dataArray4 = (double *)malloc(nnN*sizeof(double));
    if(dataArray4==NULL)
        return NULL;  //mem not enough.
    /*Open the file.*/
    fp = fopen(chrFilePath,"rb");
    if(fp==NULL)
        return NULL; //"Cannot open the file for reading image data!"
    /*skip the header and prepare for reading image data.*/
    // we do not take entire image but take the image begining at ImagePosI0,ImagePosJ0
    // fseek(fp,nshift,SEEK_SET);
    i=0;j=0;m=0l;
    /*read 8 bytes (64bits) double as a pixel value.*/
    for(j=ImagePosJ0; j<ImagePosJ0+ImageHeight; j++)
    {
        nshift = HeaderSize+j*ImageFullWidth*sizeof(double)+ImagePosI0*sizeof(double); fseek(fp,nshift,SEEK_SET);
        for(i=ImagePosI0; i<ImagePosI0+ImageWidth; i++)
        {
            if(isLittleEndian2()) //swap reading bytes order on Intel PC.
            {
                d4p = (char *)&d4;
                for(long ibyte=sizeof(double)-1; ibyte>=0; ibyte--) {
                    d4i = fread(&d4p[ibyte], 1, 1, fp);
                }
                // d4i = fread(&d4p[7], 1, 1, fp);
                // d4i = fread(&d4p[6], 1, 1, fp);
                // d4i = fread(&d4p[5], 1, 1, fp);
                // d4i = fread(&d4p[4], 1, 1, fp);
                // d4i = fread(&d4p[3], 1, 1, fp);
                // d4i = fread(&d4p[2], 1, 1, fp);
                // d4i = fread(&d4p[1], 1, 1, fp);
                // d4i = fread(&d4p[0], 1, 1, fp);
            }
            else
            {
                d4i = fread(&d4, 4, 1, fp);
            }
            dataArray4[m]=d4;
            m++;
        }
    }
    // printf("m=%ld!sizeof(double)=%ld!\n",m,sizeof(double));
    /*close the file.*/
    fclose(fp);
    return dataArray4;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read integer type (nbits=32) FITS image block.*/
int *readFitsImageBlockI32(const char *chrFilePath, int ImagePosI0, int ImagePosJ0, int ImageWidth, int ImageHeight, int ImageFullWidth, int ImageFullHeight, long HeaderSize)
{
    int   i4 = 0;
    char *i4p=NULL;//a char * pointer to i4.
    int   i4i = 0;
    int   nnX=0, nnY=0, nnN=0;
    long  nshift=0;
    int   i=0,j=0;
    long  m=0;
    int *dataArray4=NULL;
    FILE *fp;
    /*load parameters.*/
    nnX = ImageWidth;
    nnY = ImageHeight;
    nnN = nnX * nnY;
    dataArray4 = (int *)malloc(nnN*sizeof(int));
    if(dataArray4==NULL)
        return NULL;  //mem not enough.
    /*Open the file.*/
    fp = fopen(chrFilePath,"rb");
    if(fp==NULL)
        return NULL; //"Cannot open the file for reading image data!"
    /*skip the header and prepare for reading image data.*/
    // we do not take entire image but take the image begining at ImagePosI0,ImagePosJ0
    // fseek(fp,nshift,SEEK_SET);
    i=0;j=0;m=0l;
    /*read 4 bytes (32bits) integer as a pixel value.*/
    for(j=ImagePosJ0; j<ImagePosJ0+ImageHeight; j++)
    {
        nshift = HeaderSize+j*ImageFullWidth*sizeof(int)+ImagePosI0*sizeof(int); fseek(fp,nshift,SEEK_SET);
        for(i=ImagePosI0; i<ImagePosI0+ImageWidth; i++)
        {
            if(isLittleEndian2()) //swap reading bytes order on Intel PC.
            {
                i4p = (char *)&i4;
                for(long ibyte=sizeof(int)-1; ibyte>=0; ibyte--) {
                    i4i = fread(&i4p[ibyte], 1, 1, fp);
                }
                // i4i = fread(&i4p[3], 1, 1, fp);
                // i4i = fread(&i4p[2], 1, 1, fp);
                // i4i = fread(&i4p[1], 1, 1, fp);
                // i4i = fread(&i4p[0], 1, 1, fp);
            }
            else
            {
                i4i = fread(&i4, 4, 1, fp);
            }
            dataArray4[m]=i4;
            m++;
        }
    }
    // printf("m=%ld!sizeof(int)=%ld!\n",m,sizeof(int));
    fclose(fp);
    return dataArray4;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//take one pixel value from a data array
//ImageCoordX/Y starts from 1 to ImageWidth/Height
double   takePixel(double *data, int ImageCoordX, int ImageCoordY, int ImageWidth, int ImageHeight)
{
    double ValueOfPixel = 0.0;
    if(ImageCoordX>=1 && ImageCoordX<=ImageWidth && ImageCoordY>=1 && ImageCoordY<=ImageHeight) {
        ValueOfPixel = data[(ImageCoordY-1)*ImageWidth+ImageCoordX-1];
    }
    return ValueOfPixel;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//readFitsXTENSION can read EXTNAME and locate the XTENSION number of this EXTNAME.
//e.g. EXTNAME="SLWC3", then after readFitsXTENSION, we can know the extension number is 4.
int   readFitsXTENSION(const char *FilePath, const char* EXTNAME)
{
    int xtension=0;
    char *headerSection;
    char *EXTVALUE;
    int xtensionCount = readFitsHeaderExtensions(FilePath);
    int flagDone=0;
    for(int i=0; i<=xtensionCount; i++)
    {
        headerSection = readFitsHeader(FilePath,i);      //mem alloc
        if(strlen(headerSection)<=1)
            flagDone=1;
        EXTVALUE = extKeyword("EXTNAME",headerSection);  //mem alloc
        if(EXTVALUE!=NULL && 0==strncmp(EXTVALUE,EXTNAME,strlen(EXTNAME)) )
        {
            xtension=i;
            flagDone=1;
        }
        free(EXTVALUE);                                  //mem free
        free(headerSection);                             //mem free
        if(flagDone)
            break;
    }
    if(xtension>0)
        return xtension;
    return -1;
}

//readFitsBintable actually returns BIGENDIAN data, thus when translate char * into
//typical formats of data, we need to swap bytes for each double or int.
void *readFitsBintable(const char *FilePath, int xtension, int *DataXSize, int *DataYSize)
{
    void *DataArray=NULL;
    char *DataFormat=NULL;
    readFitsBintable(FilePath, xtension, &DataArray, &DataFormat, DataXSize, DataYSize);
    return (void *)DataArray;
}

void *readFitsBintable(const char *FilePath, const char* EXTNAME, int *DataXSize, int *DataYSize)
{//using EXTNAME as bintable name.
    void *DataArray=NULL;
    char *DataFormat=NULL;
    int  xtension = readFitsXTENSION(FilePath,EXTNAME);
//    int xtension=0;
//    char *headerSection;
//    char *EXTVALUE;
//    int xtensionCount = readFitsHeaderExtensions(FilePath);
//    int flagDone=0;
//    for(int i=0; i<=xtensionCount; i++)
//    {
//        headerSection = readFitsHeader(FilePath,i);      //mem alloc
//        if(strlen(headerSection)<=1)
//            flagDone=1;
//        EXTVALUE = extKeyword("EXTNAME",headerSection);  //mem alloc
//        if(EXTVALUE!=NULL && 0==strncmp(EXTVALUE,EXTNAME,strlen(EXTNAME)) )
//        {
//            xtension=i;
//            flagDone=1;
//        }
//        free(EXTVALUE);                                  //mem free
//        free(headerSection);                             //mem free
//        if(flagDone)
//            break;
//    }
    if(xtension>0)
    {
        readFitsBintable(FilePath, xtension, &DataArray, &DataFormat, DataXSize, DataYSize);
        return (void *)DataArray;
    }
    else
        return NULL;
}

/*core function to read bintable.*/
/*Supported: Herschel fits; */
int readFitsBintable(const char *FilePath, int xtension, void **DataArray, char **DataFormat, \
                     int *DataXSize, int *DataYSize)
{
    int errorcode=0;
    char *HeaderText=NULL;
    long HeaderPosition=0,HeaderSize=0;
    errorcode = readFitsHeader(FilePath,xtension,&HeaderText,&HeaderPosition,&HeaderSize);
    if (errorcode!=0) { printf("readFitsBintable: Error! Could not get fits header from readFitsHeader!\n"); errorcode=-1; return errorcode; }
    char *strKey_XTENSION = extKeyword("XTENSION",HeaderText);
    char *strKey_NAXIS = extKeyword("NAXIS",HeaderText);
    char *strKey_NAXIS1 = NULL;
    char *strKey_NAXIS2 = NULL;
    int   nnX=0,nnY=0;
    //only handle bintable.
    if(strncmp(strKey_XTENSION,"BINTABLE",8) != 0)
    {
        errorcode = 3;
        printf("myfitsIO : readFitsBintable : No BINTABLE found.\n");
        return errorcode;
    }
    //only handle 2-dimension.
    if(strKey_NAXIS && strKey_NAXIS[0]=='2')
    {
        strKey_NAXIS1 = extKeyword("NAXIS1",HeaderText);
        strKey_NAXIS2 = extKeyword("NAXIS2",HeaderText);
        nnX = atoi(strKey_NAXIS1);
        nnY = atoi(strKey_NAXIS2);
    }
    //error check
    if(nnX<=0 || nnY<=0)
    {
        printf("myfitsIO : readFitsBintable : No NAXIS found.\n");
        *DataArray=NULL;
        *DataFormat=NULL;
        *DataXSize=0;
        *DataYSize=0;
        return -1;
    }
    if(DataXSize==NULL || DataYSize==NULL)
        return -1;
    //find out the unit format.
    //e.g. Herschel SLWC3 header -- TFORM1='1D';TFORM2='1D';TFORM3='1D';TFORM4='1J' => DataFormat=="1D,1D,1D,1J,"
    char *DataFormatPtr=NULL;
    char *DataFormatKey=NULL;
    char *DataFormatStr=NULL;
    int   DataFormatInx=0;
    DataFormatKey = (char *)malloc(7*sizeof(char)); //DataFormatKey=="TFORM1"
    if(DataFormatKey==NULL)
    {
        printf("myfitsIO : readFitsBintable : DataFormatKey array cannot be allocated.\n");
        return -1;
    }
    memset(DataFormatKey,0x0,7);
    DataFormatKey[0]='T';DataFormatKey[1]='F';DataFormatKey[2]='O';DataFormatKey[3]='R';DataFormatKey[4]='M';
    DataFormatPtr = (char *)malloc(128*sizeof(char));
    if(DataFormatPtr==NULL)
    {
        free(DataFormatKey);
        printf("myfitsIO : readFitsBintable : DataFormatPtr array cannot be allocated.\n");
        return -1;
    }
    memset(DataFormatPtr,0x0,128);
    for(int i=1; i<10; i++) //TFORM at most 9, which means columns at most 10.
    {
        DataFormatKey[5] = '0'+i; //makes the keyword "TFORMi"
        DataFormatStr = extKeyword(DataFormatKey, HeaderText);
        if(DataFormatStr!=NULL)
        {
            strncpy(DataFormatPtr+DataFormatInx, DataFormatStr, strlen(DataFormatStr));
            DataFormatInx += strlen(DataFormatStr);
            DataFormatPtr[DataFormatInx++]=',';
        }
        else break;
    }
    free(DataFormatKey);
    if(strlen(DataFormatPtr)<5)
    {
        printf("myfitsIO : readFitsBintable : Bintable Does not contain >2 columns.\n");
        return -1;
    }
    //malloc the void * type data arry.
    void *DataArrayPtr=NULL;
    DataArrayPtr = (void *)malloc(nnX*nnY*sizeof(char));
    if(DataArrayPtr==NULL)
    {
        free(DataFormatPtr);
        printf("myfitsIO : readFitsBintable : DataFormat array cannot be allocated.\n");
        return -1;
    }
    memset(DataArrayPtr,0x0,nnX*nnY);
    //read fits file.
    FILE *fp;
    fp = fopen(FilePath,"rb");
    if(fp==NULL)
    {
        free(DataFormatPtr);
        printf("myfitsIO : readFitsBintable : File cannot be read.\n");
        return -5;
    }
    fseek(fp,(long)(HeaderPosition+HeaderSize),SEEK_SET);
    fread(DataArrayPtr,nnX,nnY,fp);
    fclose(fp);
    //save results.
    *DataArray  = DataArrayPtr;
    *DataFormat = DataFormatPtr;
    *DataXSize = nnX;
    *DataYSize = nnY;
    return 0;
}

////<TODO: for herschel only>
//int readFitsBintable_Beta(const char *FilePath, int xtension, void **DataArray, char **DataFormat, \
//                     int *DataXSize, int *DataYSize)
//{
//    int errorcode=0;
//    FILE *fp;
//    char *HeaderText=NULL;
//    int HeaderPosition=0,HeaderSize=0;
//    errorcode = readFitsHeader(FilePath,xtension,&HeaderText,&HeaderPosition,&HeaderSize);
//    char *strKey_XTENSION = extKeyword("XTENSION",HeaderText);
//    char *strKey_NAXIS = extKeyword("NAXIS",HeaderText);
//    char *strKey_NAXIS1 = NULL;
//    char *strKey_NAXIS2 = NULL;
//    int   nnX=0,nnY=0;
//    //only handle bintable.
//    if(strncmp(strKey_XTENSION,"BINTABLE",8) != 0)
//    {
//        errorcode = 3;
//        return errorcode;
//    }
//    //only handle 2-dimension.
//    if(strKey_NAXIS && strKey_NAXIS[0]=='2')
//    {
//        strKey_NAXIS1 = extKeyword("NAXIS1",HeaderText);
//        strKey_NAXIS2 = extKeyword("NAXIS2",HeaderText);
//        nnX = atoi(strKey_NAXIS1);
//        nnY = atoi(strKey_NAXIS2);
//    }
//    //malloc the void * type data arry.
//    if(nnX && nnY)
//        (*DataArray) = (void *)malloc(nnX*nnY*sizeof(char));
//    //read fits file.
//    fp = fopen(FilePath,"rb");
//    if(fp==NULL)
//    {
//        errorcode = 5;
//        return errorcode;
//    }
//    fseek(fp,(long)(HeaderPosition+HeaderSize),SEEK_SET);
//    fread((*DataArray),nnX,nnY,fp);
//    fclose(fp);
//    //TODO: DataFormat ??
//    //      TFORM1,TFORM2
//    if(DataXSize==NULL || DataYSize==NULL)
//        return -1;
//    *DataXSize = nnX;
//    *DataYSize = nnY;
//    return errorcode;
//}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Read FITS Spectrum.*/
int     readFitsSpectrumRows(const char *FilePath, const char *EXTNAME)
{
    int nnCol=0;
    int nnRow=0;
    char *HeaderText=NULL;
    long HeaderPosition=0,HeaderSize=0;
    int xtension = readFitsXTENSION(FilePath,EXTNAME);
    if(xtension<0)
    {
        printf("readFitsSpectrumRows()  Fits file %s does not contain EXTNAME=%s.\n",FilePath,EXTNAME);
        return -1;
    }
    int errorcode = readFitsHeader(FilePath,xtension,&HeaderText,&HeaderPosition,&HeaderSize);
    if(errorcode!=0)
    {
        printf("readFitsSpectrumRows()  Fits file %s can not be read with EXTNAME=%s.\n",FilePath,EXTNAME);
        return -1;
    }
    //char *strKey_XTENSION = extKeyword("XTENSION",HeaderText); //this should be equal to EXTNAME.
    //char *strKey_NAXIS = extKeyword("NAXIS",HeaderText);
    char *strKey_NAXIS1 = extKeyword("NAXIS1",HeaderText);
    char *strKey_NAXIS2 = extKeyword("NAXIS2",HeaderText);
    nnCol = atoi(strKey_NAXIS1);
    nnRow = atoi(strKey_NAXIS2);
    return nnRow;
}

double *readFitsSpectrum(const char *FilePath, const char *EXTNAME)
{
    int nnCol=0;
    int nnRow=0;
    void *binData=NULL;
    char *binFormat=NULL;
    int xtension = readFitsXTENSION(FilePath,EXTNAME);
    //<TEST> printf("readFitsSpectrum()  readFitsXTENSION() xtension=%d.\n",xtension);
    if(xtension<0) return NULL;
    int errorcode = readFitsBintable(FilePath,xtension,&binData,&binFormat,&nnCol,&nnRow); //执行这一步时出了问题.
    //<TEST> printf("readFitsSpectrum()  readFitsBintable() nnRow=%d.\n",nnRow);
    if(errorcode!=0) return NULL;
    if(nnCol<=0 || nnRow<=0) return NULL;
    //now binFormat=="1D,1D,1D,1J".
    //<TEST> printf("readFitsSpectrum()  binFormat=%s.\n",binFormat);
    char   *xFormat = strtok(binFormat,","); //run strtok once, it returns the first splitted string.;
    char   *yFormat = strtok(NULL,","); //run strtok again with NULL input, it returns the second split.
    void   *binDataPtr = binData;
    double *DataArray = (double *)malloc(nnRow*2*sizeof(double)); //x0,y0,x1,y1,x2,y2,x3,y3,...
    //<TEST> printf("readFitsSpectrum()  DataArray has been memory-allocated.\n");
    //<TODO: we can not deal with x,y not being double...>
    //<TODO: here we only deal with "1D,1D,...">
    if(strncmp(xFormat,"1D",2)!=0 || strncmp(yFormat,"1D",2)!=0)
        return NULL;
    for(int j=0; j<nnRow; j++)
    {
        //binData contains : x,y,z,t
        //binData should be bytes swapped.
        binDataPtr = (char *)binData + nnCol*j;
        char *xValueStr = inverseBytes((char *)binDataPtr,8);
        double *xValue = (double *)xValueStr;
        binDataPtr = (char *)binDataPtr + 8;
        char *yValueStr = inverseBytes((char *)binDataPtr,8);
        double *yValue = (double *)yValueStr;
        DataArray[2*j]   = *xValue;
        DataArray[2*j+1] = *yValue;
        free(xValueStr);
        free(yValueStr);
    }
    return DataArray;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int isLittleEndian2()
{
    int isLittleEndian2=1;

    /*decide the system's endian.*/
    union{short s; char c[2];}decideEndian;

    //TODO: what if sizeof(short) != 2 ??
    if(sizeof(short) == 2)
    {
        decideEndian.s = 0x0102;
        if(decideEndian.c[0] == 1 && decideEndian.c[1] == 2) //big enidan
            isLittleEndian2=0;
        else if(decideEndian.c[0] == 2 && decideEndian.c[1] == 1) //little endian
            isLittleEndian2=1;
    }

    return isLittleEndian2;
}

char *inverseBytes(char *content, int length)
{
    int i=0,j=0;
    char *back;
    back = (char *)malloc(length*sizeof(char));
    memset(back,0x00,length);
    for(i=length-1,j=0; i>=0; i--,j++)
    {
        back[j]=content[i];
    }
    return back;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Write float type FITS.*/
int writeFitsF(float *data, char *header, const char *strFilePath) //TODO:TCHAR?
{
                                  //errorcode - 61 输入参数无法获取.
                                  //errorcode - 62 输入参数与本函数不匹配.
                                  //errorcode - 50 cannot open file for r/w. unknow reason.
                                  //errorcode - 53 file has been opened but cannot write.
    int errorcode = 0;
    int DataWidth=0,DataHeight=0,DataBits=0;
    int flag=0,m=0;
    char *cWidth,*cHeight,*cBits;
    FILE *fp;
    char *dp; //data pointer

    cWidth = extKeyword("NAXIS1",header);
    cHeight = extKeyword("NAXIS2",header);
    cBits = extKeyword("BITPIX",header);
    DataWidth = atoi(cWidth);
    DataHeight = atoi(cHeight);
    DataBits = atoi(cBits);
    if(DataWidth==0 || DataHeight==0 || DataBits==0) { errorcode=61; return errorcode;}
    if(DataBits!=-32) { errorcode=62; return errorcode;}
    /*write fits header.*/
    fp=fopen(strFilePath,"wb"); //TODO: support Chinese Path?
    if(fp==NULL)      { errorcode=5; return errorcode;}
    flag = fwrite(header,strlen(header),1,fp);
    flag = checkHead2880(strlen(header),fp); //<Added><20141119><DzLIU>
    /*write data. deciding whether swap bytes.*/
    if(isLittleEndian2())
    {
        for(m=0;m<DataWidth*DataHeight;m++,dp+=4)
        {
            dp = (char *)&data[m];
            flag = fwrite(&dp[3], 1, 1, fp);
            flag = fwrite(&dp[2], 1, 1, fp);
            flag = fwrite(&dp[1], 1, 1, fp);
            flag = fwrite(&dp[0], 1, 1, fp);
        }
        int flagData = checkData2880(4*DataWidth*DataHeight,fp); //<Added><20141119><DzLIU>
    }
    else
    {
        flag = fwrite(data, 4*DataWidth*DataHeight, 1, fp);
        int flagData = checkData2880(4*DataWidth*DataHeight,fp); //<Added><20141119><DzLIU>
    }
    fclose(fp);
    if(flag!=1) { errorcode=53; return errorcode; }
    return errorcode;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Write double type FITS.*/
int writeFitsD(double *data, char *header, const char *strFilePath) //TODO:TCHAR?
{
                                  //errorcode - 61 输入参数无法获取.
                                  //errorcode - 62 输入参数与本函数不匹配.
                                  //errorcode - 50 cannot open file for r/w. unknow reason.
                                  //errorcode - 53 file has been opened but cannot write.
    int errorcode = 0;
    int debugcode = 0;
    long DataWidth=0,DataHeight=0,DataBits=0;
    int flag=0,m=0;
    char *cWidth,*cHeight,*cBits;
    FILE *fp;
    char *dp; //data pointer

    cWidth = extKeyword("NAXIS1",header);
    cHeight = extKeyword("NAXIS2",header);
    cBits = extKeyword("BITPIX",header);
    //printf("writeFitsD: NAXIS1 = %s\n",cWidth);
    //printf("writeFitsD: NAXIS2 = %s\n",cHeight);
    //printf("writeFitsD: BITPIX = %s\n",cBits);
    DataWidth = atol(cWidth);
    DataHeight = atol(cHeight);
    DataBits = atol(cBits);
    //printf("writeFitsD: NAXIS1 = %ld\n",DataWidth);
    //printf("writeFitsD: NAXIS2 = %ld\n",DataHeight);
    //printf("writeFitsD: BITPIX = %ld\n",DataBits);
    if(DataWidth==0 || DataHeight==0 || DataBits==0) { errorcode=61; return errorcode;}
    if(DataBits!=-64) { errorcode=62; return errorcode;}
    /*write fits header.*/
    fp=fopen(strFilePath,"wb"); //TODO: support Chinese Path?
    if(fp==NULL) { printf("writeFitsD: Failed to open %s\n",strFilePath); errorcode=5; return errorcode; }
    if(debugcode>=1) { printf("writeFitsD: opening %s to write\n",strFilePath); }
    flag = fwrite(header,strlen(header),1,fp);
    if(debugcode>=1) { printf("writeFitsD: writing fits header of %ld bytes\n",strlen(header)); }
    flag = checkHead2880(strlen(header),fp); //<Added><20141119><DzLIU>
    if(debugcode>=1) { printf("writeFitsD: writing fits header of %ld bytes\n",strlen(header)+flag); }
    /*write data. deciding whether swap bytes.*/
    if(isLittleEndian2())
    {
        if(debugcode>=1) { printf("writeFitsD: writing fits image of %ldx%ld pixels\n",DataWidth,DataHeight); }
        for(m=0;m<DataWidth*DataHeight;m++,dp+=8)
        {
            dp = (char *)&data[m];
            // printf("writeFitsD: debugging data[%d] %-15lf %02X %02X %02X %02X %02X %02X %02X %02X\n",m,data[m],(unsigned)(unsigned char)dp[7],(unsigned)(unsigned char)dp[6],(unsigned)(unsigned char)dp[5],(unsigned)(unsigned char)dp[4],(unsigned)(unsigned char)dp[3],(unsigned)(unsigned char)dp[2],(unsigned)(unsigned char)dp[1],(unsigned)(unsigned char)dp[0]); /* about printf single byte as 02X, see http://stackoverflow.com/questions/3555791/why-does-printf-not-print-out-just-one-byte-when-printing-hex */
            if(debugcode>=1) {
                printf("writeFitsD: debugging data[%d] (%0ld,%0ld) %-15lf ",m,m%DataWidth,m/DataWidth,data[m]);
                printf(" %02X",(unsigned)(unsigned char)dp[7]);
                printf(" %02X",(unsigned)(unsigned char)dp[6]);
                printf(" %02X",(unsigned)(unsigned char)dp[5]);
                printf(" %02X",(unsigned)(unsigned char)dp[4]);
                printf(" %02X",(unsigned)(unsigned char)dp[3]);
                printf(" %02X",(unsigned)(unsigned char)dp[2]);
                printf(" %02X",(unsigned)(unsigned char)dp[1]);
                printf(" %02X",(unsigned)(unsigned char)dp[0]);
                // printf("\n");
            }
            flag = fwrite(&dp[7], 1, 1, fp);
            flag = fwrite(&dp[6], 1, 1, fp);
            flag = fwrite(&dp[5], 1, 1, fp);
            flag = fwrite(&dp[4], 1, 1, fp);
            flag = fwrite(&dp[3], 1, 1, fp);
            flag = fwrite(&dp[2], 1, 1, fp);
            flag = fwrite(&dp[1], 1, 1, fp);
            flag = fwrite(&dp[0], 1, 1, fp);
            if(debugcode>=1) { printf("   copied!\n"); }
        }
        int flagData = checkData2880(8*DataWidth*DataHeight,fp); //<Added><20141119><DzLIU>
    }
    else
    {
        if(debugcode>=1) { printf("writeFitsD: writing fits image of %ldx%ld pixels (big endian)\n",DataWidth,DataHeight); }
        flag = fwrite(data, 8*DataWidth*DataHeight, 1, fp);
        int flagData = checkData2880(8*DataWidth*DataHeight,fp); //<Added><20141119><DzLIU>
    }
    fclose(fp); if(debugcode>=1) { printf("writeFitsD: debugging flag=%d errorcode=%d!\n",flag,errorcode); }
    if(flag!=1) { errorcode=53; return errorcode; }
    return errorcode;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Write a simple float type FITS.*/
int writeFitsFS(float *data, int DataWidth, int DataHeight, const char *strFilePath) //TCHAR
{
    int errorcode = 0;
    int flag=0,m=0;
    FILE *fp;
    char *header;
    char *generateFitsHeaderFS(int DataWidth, int DataHeight);
    char *dp;

    fp=fopen(strFilePath,"wb"); //TODO: support Chinese Path?
    if(fp==NULL) { errorcode=5; return errorcode;}
    header = generateFitsHeaderFS(DataWidth,DataHeight);
    flag = fwrite(header,36*80,1,fp);
    free((void *)header);
    /*swap bytes.*/
    if(isLittleEndian2())
    {
        for(m=0;m<DataWidth*DataHeight;m++,dp+=4)
        {
            dp = (char *)&data[m];
            flag = fwrite(&dp[3], 1, 1, fp);
            flag = fwrite(&dp[2], 1, 1, fp);
            flag = fwrite(&dp[1], 1, 1, fp);
            flag = fwrite(&dp[0], 1, 1, fp);
        }
    }
    else
    {
        flag = fwrite(data, 4*DataWidth*DataHeight, 1, fp);
    }

    fclose(fp);
    if(flag!=1) { errorcode=53; return errorcode; }
    return errorcode;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Write a simple double type FITS.*/
int writeFitsDS(double *data, int DataWidth, int DataHeight, const char *strFilePath) //TCHAR
{
    int errorcode = 0;
    int flag=0,m=0;
    FILE *fp;
    char *header;
    char *generateFitsHeaderDS(int DataWidth, int DataHeight);
    char *dp;

    fp=fopen(strFilePath,"wb"); //TODO: support Chinese Path?
    if(fp==NULL) { errorcode=5; return errorcode;}
    header = generateFitsHeaderDS(DataWidth,DataHeight);
    flag = fwrite(header,36*80,1,fp);
    free((void *)header);
    /*swap bytes.*/
    if(isLittleEndian2())
    {
        for(m=0;m<DataWidth*DataHeight;m++,dp+=4)
        {
            dp = (char *)&data[m];
            flag = fwrite(&dp[7], 1, 1, fp);
            flag = fwrite(&dp[6], 1, 1, fp);
            flag = fwrite(&dp[5], 1, 1, fp);
            flag = fwrite(&dp[4], 1, 1, fp);
            flag = fwrite(&dp[3], 1, 1, fp);
            flag = fwrite(&dp[2], 1, 1, fp);
            flag = fwrite(&dp[1], 1, 1, fp);
            flag = fwrite(&dp[0], 1, 1, fp);
        }
    }
    else
    {
        flag = fwrite(data, 8*DataWidth*DataHeight, 1, fp);
    }

    fclose(fp);
    if(flag!=1) { errorcode=53; return errorcode; }
    return errorcode;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*fits head must be the integer times of 2880 bytes.*/
int checkHead2880(long headBytes, FILE* writeToFile)
{
    long padHead2880 = 2880 - (headBytes % 2880);
    if(padHead2880==2880) { padHead2880 = 0; }
    //printf("padHead2880=%ld\n",padHead2880);
    if(writeToFile) {
        for(int writeCount=0; writeCount<padHead2880; writeCount++) {
            char writeByte = 0x20;
            int writeCheck = fwrite(&writeByte, 1, 1, writeToFile);
        }
    }
    return padHead2880;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*fits data must be the integer times of 2880 bytes.*/
int checkData2880(long dataBytes, FILE* writeToFile)
{
    long padData2880 = 2880 - (dataBytes % 2880);
    if(padData2880==2880) { padData2880 = 0; }
    //printf("padData2880=%ld\n",padData2880);
    if(writeToFile) {
        for(int writeCount=0; writeCount<padData2880; writeCount++) {
            char writeByte = 0x00;
            int writeCheck = fwrite(&writeByte, 1, 1, writeToFile);
        }
    }
    return padData2880;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*generateFitsHeader Float(-32bits) Simplest(contains no RA,Dec info).*/
//this function is called by the writeFitsFS automatically.
char *generateFitsHeaderFS(int DataWidth, int DataHeight)
{
   int   i=0,j=0,m=0,temp1=0,temp2=0,nnX=0,nnY=0;
   char line1[81] ="SIMPLE  =                    T / Fits standard                                  "; //actually it's 80 bytes.
   char line2[81] ="BITPIX  =                  -32 / Bits per pixel                                 "; //But the extra 1bytes is
   char line3[81] ="NAXIS   =                    2 / Number of axes                                 "; //used for '\0'.
   char line4[81] ="NAXIS1  =                    X / Axis length                                    ";
   char line5[81] ="NAXIS2  =                    Y / Axis length                                    ";
   char line6[81] ="EXTEND  =                    F / File may contain extensions                    ";
   //char line7[81] ="                                                                                ";
   char line36[81]="END                                                                             ";
   char chrnnX[16]="               "; //<corrected><20150227><dzliu> 15 white spaces but char array needs 16 bytes. 
   char chrnnY[16]="               "; //<corrected><20150227><dzliu> 15 white spaces but char array needs 16 bytes. 
   char *header=NULL;
   
   nnX = DataWidth;
   nnY = DataHeight;
   //itoa(nnX,chrnnX); //some unix does not contain the itoa() function.
   sprintf(chrnnX,"%15d",nnX);
   sprintf(chrnnY,"%15d",nnY);
   temp1=strlen(chrnnX);
   temp2=strlen(chrnnY);
   //<debug><20150227><dzliu>printf("NAXIS1-sprintf=%s NAXIS1-strlen=%d(%d)\n",chrnnX,temp1,DataWidth);
   //<debug><20150227><dzliu>printf("NAXIS2-sprintf=%s NAXIS2-strlen=%d(%d)\n",chrnnY,temp2,DataHeight);
   for(i=0;i<temp1;i++)
   {
       line4[29-i]=chrnnX[temp1-1-i];
   }
   for(j=0;j<temp2;j++)
   {
       line5[29-j]=chrnnY[temp2-1-j];
   }
   header = (char *)malloc((36*80+1)*sizeof(char)); //36 lines, each line is 80 bytes. //ending byte corrected 20150309 dzliu
   memset(header,0x20,(36*80+1)); header[(36*80)]=0x0;
   for(m=0 ,i=0;i<80;m++,i++) header[m]=line1[i];
   for(m=80,i=0;i<80;m++,i++) header[m]=line2[i];
   for(m=2*80,i=0;i<80;m++,i++) header[m]=line3[i];
   for(m=3*80,i=0;i<80;m++,i++) header[m]=line4[i];
   for(m=4*80,i=0;i<80;m++,i++) header[m]=line5[i];
   for(m=5*80,i=0;i<80;m++,i++) header[m]=line6[i];
   for(m=6*80;m<35*80 ;m++,i++) header[m]=0x20;
   for(m=35*80,i=0;i<80;m++,i++) header[m]=line36[i];
   
   return header;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*generateFitsHeader double(-64bits) Simplest(contains no RA,Dec info).*/
char *generateFitsHeaderDS(int DataWidth, int DataHeight)
{
   int   i=0,j=0,m=0,temp1=0,temp2=0,nnX=998,nnY=1006;
   char line1[81] ="SIMPLE  =                    T / Fits standard                                  "; //actually it's 80 bytes.
   char line2[81] ="BITPIX  =                  -64 / Bits per pixel                                 ";//But the extra 1bytes is
   char line3[81] ="NAXIS   =                    2 / Number of axes                                 ";//used for '\0'.
   char line4[81] ="NAXIS1  =                    X / Axis length                                    ";
   char line5[81] ="NAXIS2  =                    Y / Axis length                                    ";
   char line6[81] ="EXTEND  =                    F / File may contain extensions                    ";
   //char line7[81] ="                                                                                ";
   char line36[81]="END                                                                             ";
   char chrnnX[7];
   char chrnnY[7];
   char *header=NULL;
   // const char *header2;

   nnX = DataWidth;
   nnY = DataHeight;
   //itoa(nnX,chrnnX); //some unix does not contain the itoa() function.
   sprintf(chrnnX,"%d",nnX);
   sprintf(chrnnY,"%d",nnY);
   temp1=strlen(chrnnX);
   temp2=strlen(chrnnY);
   for(i=0;i<temp1;i++)
   {
       line4[29-i]=chrnnX[temp1-1-i];
   }
   for(j=0;j<temp2;j++)
   {
       line5[29-j]=chrnnY[temp2-1-j];
   }
   header = (char *)malloc(36*80*sizeof(char)); //36 lines, each line is 80 bytes.
   for(m=0 ,i=0;i<80;m++,i++) header[m]=line1[i];
   for(m=80,i=0;i<80;m++,i++) header[m]=line2[i];
   for(m=2*80,i=0;i<80;m++,i++) header[m]=line3[i];
   for(m=3*80,i=0;i<80;m++,i++) header[m]=line4[i];
   for(m=4*80,i=0;i<80;m++,i++) header[m]=line5[i];
   for(m=5*80,i=0;i<80;m++,i++) header[m]=line6[i];
   for(m=6*80;m<35*80 ;m++,i++) header[m]=0x20;
   for(m=35*80,i=0;i<80;m++,i++) header[m]=line36[i];

   // header2 = (const char *)header;

   return header;
}


























/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*Modify FITS at a certain insert position (edited 2017-08-03).*/
/*Note that the input InsertPosI0 and InsertPosJ0 are the pixel coordinate starting from 0 to NAXIS1-1 and NAXIS2-1.*/
int modifyFitsImage(double *data, int DataWidth, int DataHeight, int InsertPosI0, int InsertPosJ0, const char *strFilePath, int xtension, int debugcode) //TCHAR
{
    int errorcode = 0;
    int i=0,j=0,k=0;
    FILE *fp;
    char *dp;
    
    /*Read fits header.*/
    long    HeaderPos  = 0;
    long    HeaderSize = 0;
    char   *HeaderData;
    errorcode = readFitsHeader(strFilePath,xtension,&HeaderData,&HeaderPos,&HeaderSize);
    if(errorcode!=0) {
        printf("modifyFitsImage: Error! Could not get fits header from readFitsHeader!\n"); return errorcode;
    } else {
        if(debugcode>=1) {
            printf("modifyFitsImage: FitsHeaderPos %ld, FitsHeaderSize %ld.\n", HeaderPos, HeaderSize);
        }
    }
    
    /*Read fits header keywords.*/
    char   *BITPIX = extKeyword("BITPIX",HeaderData);
    char   *NAXIS1 = extKeyword("NAXIS1",HeaderData);
    char   *NAXIS2 = extKeyword("NAXIS2",HeaderData);
    int     ImageBitpix = atoi(BITPIX);
    int     ImageWidth  = atoi(NAXIS1);
    int     ImageHeight = atoi(NAXIS2);
    int     ImageSize = ImageWidth*ImageHeight;
    if(ImageWidth<=0 || ImageHeight<=0) { printf("modifyFitsImage: Error! Could not get NAXIS from fits header!\n"); return errorcode; }
    
    /*Check the insert position.*/
    int InsertWidth = DataWidth; // the exact image block to be inserted
    int InsertHeight = DataHeight; // the exact image block to be inserted
    int DataPosI0 = 0; // the input data block starting Position
    int DataPosJ0 = 0;
    if(InsertPosI0<0) { DataPosI0 += (-InsertPosI0); InsertWidth -= (-InsertPosI0); InsertPosI0 = 0; }
    if(InsertPosJ0<0) { DataPosJ0 += (-InsertPosJ0); InsertHeight -= (-InsertPosJ0); InsertPosJ0 = 0; }
    // e.g., FitsImage 3x3, DataSize 2x2, InsertPos (-1,-1), i.e. one pixel bottom-left outer the fits image,
    // ----> DataPos (1,1), InsertSize 1x1, InsertPos (0,0).
    if(InsertPosI0+InsertWidth>ImageWidth) { InsertWidth -= (InsertPosI0+InsertWidth-ImageWidth); }
    if(InsertPosJ0+InsertHeight>ImageHeight) { InsertHeight -= (InsertPosJ0+InsertHeight-ImageHeight); }
    // e.g., FitsImage 4x4, DataSize 3x3, InsertPos (2,2), i.e. one pixels top-right outer the fits image,
    // ----> DataPos unchanged, InsertSize 1x1, InsertPos unchanged.
    
    /*Check the insert width and height.*/
    if(InsertWidth<=0 || InsertHeight<=0) {
        printf("modifyFitsImage: Warning! Input image block is out of the field of the target image, no data copied.\n"); return errorcode;
    } else {
        if(debugcode>=1) {
            printf("modifyFitsImage: DataSize %dx%d, DataPos (%d,%d), InsertSize %dx%d, FitsImageSize %dx%d, FitsImageInsertPos (%d,%d).\n", DataWidth, DataHeight, DataPosI0, DataPosJ0, ImageWidth, ImageHeight, InsertWidth, InsertHeight, InsertPosI0, InsertPosJ0);
        }
        if(debugcode>=1) {
            printf("modifyFitsImage: isLittleEndian = %d.\n", isLittleEndian2());
        }
    }
    
    /*Open fits file for modifying binary data without changing file size.*/
    fp=fopen(strFilePath,"r+b");
    if(fp==NULL) { errorcode=5; return errorcode; } // errorcode 5, failed to read file with fopen().
    
    /*Modify fits image data bytes.*/
    if(ImageBitpix==32) // int data type
    {
        for(j=0; j<InsertHeight; j++) {
            /*Move file pointer to the insert position.*/
            fseek(fp, HeaderPos+HeaderSize+((InsertPosJ0+j)*ImageWidth+(InsertPosI0))*sizeof(int), SEEK_SET);
            if(debugcode>=2) {
                printf("modifyFitsImage: fseek %ld.\n", HeaderPos+HeaderSize+((InsertPosJ0+j)*ImageWidth+(InsertPosI0))*sizeof(int));
            }
            for(i=0; i<InsertWidth; i++) {
                /*Copy data.*/
                int dv = (int)(data[(DataPosJ0+j)*DataWidth+(DataPosI0+i)]);
                /*swap bytes if LittleEndian.*/
                dp = (char *)&dv; if(isLittleEndian2()) { for(k=sizeof(int)-1; k>=0; k--) { fwrite(&dp[k],1,1,fp); } } else { fwrite(dp,sizeof(int),1,fp); }
            }
        }
    }
    else if(ImageBitpix==-32) // float data type
    {
        for(j=0; j<InsertHeight; j++) {
            /*Move file pointer to the insert position.*/
            fseek(fp, HeaderPos+HeaderSize+((InsertPosJ0+j)*ImageWidth+(InsertPosI0))*sizeof(float), SEEK_SET);
            if(debugcode>=2) {
                printf("modifyFitsImage: fseek %ld.\n", HeaderPos+HeaderSize+((InsertPosJ0+j)*ImageWidth+(InsertPosI0))*sizeof(float));
            }
            for(i=0; i<InsertWidth; i++) {
                /*Copy data.*/
                float dv = (float)(data[(DataPosJ0+j)*DataWidth+(DataPosI0+i)]);
                /*swap bytes if LittleEndian.*/
                dp = (char *)&dv; if(isLittleEndian2()) { for(k=sizeof(float)-1; k>=0; k--) { fwrite(&dp[k],1,1,fp); } } else { fwrite(dp,sizeof(float),1,fp); }
            }
        }
    }
    else if(ImageBitpix==-64) // double data type
    {
        for(j=0; j<InsertHeight; j++) {
            /*Move file pointer to the insert position.*/
            fseek(fp, HeaderPos+HeaderSize+((InsertPosJ0+j)*ImageWidth+(InsertPosI0))*sizeof(double), SEEK_SET);
            if(debugcode>=2) {
                printf("modifyFitsImage: fseek %ld.\n", HeaderPos+HeaderSize+((InsertPosJ0+j)*ImageWidth+(InsertPosI0))*sizeof(double));
            }
            for(i=0; i<InsertWidth; i++) {
                /*Copy data.*/
                double dv = (double)(data[(DataPosJ0+j)*DataWidth+(DataPosI0+i)]);
                /*swap bytes if LittleEndian.*/
                dp = (char *)&dv; if(isLittleEndian2()) { for(k=sizeof(double)-1; k>=0; k--) { fwrite(&dp[k],1,1,fp); } } else { fwrite(dp,sizeof(double),1,fp); }
            }
        }
    }
    
    fclose(fp);
    return errorcode;
}





































