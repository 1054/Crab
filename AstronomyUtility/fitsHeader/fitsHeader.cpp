#include "myfitsIO.h"
#include "fitsHeader.h"
#include "ui_fitsHeader.h"

fitsHeader::fitsHeader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fitsHeader)
{
    ui->setupUi(this);
    errorcode=0;
    m_TextHL=NULL;
    m_TextSD=NULL;
    qDebug()<<"!m_TextSD"<<!m_TextSD;
    ui->searchButton->setDisabled(true);
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->openButton,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(ui->searchButton,SIGNAL(clicked()),this,SLOT(searchText()));
}

fitsHeader::~fitsHeader()
{
    delete ui;
}


void fitsHeader::openFile()
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


void fitsHeader::openFile(QString strFilePath)
{
    int sections=0;
    int i=0,j=0;
    QString seperator1;
    QString seperator2;
    QString strsection;
    QByteArray baLine;
    seperator1.resize(80);
    seperator1.fill('=');
    seperator1.append('\n');
    seperator2.resize(80);
    seperator2.fill(' ');
    seperator2.append('\n');
    FilePath=strFilePath;
    /*Read fits header extensions' count.*/
    sections = readFitsHeaderExtensions(FilePath.toLocal8Bit().data());
    qDebug()<<"This fits contains "<<sections+1<<"sections";
    m_Text.clear();
    for(i=0; i<=sections; i++)
    {
        /*Read fits header as char * and then convert to QString.*/
        m_Data = readFitsHeader(FilePath.toLocal8Bit().data(),i);
//Note:  readFitsHeader returns a (m_Size+1) char array , with the last byte of 0x0,
//Note:  because thus strlen(m_Data) == m_Size.
        m_Size = strlen(m_Data);
        if(i>0)
        {
            m_Text.append(seperator1);
            strsection = QString("XTENSION %1").arg(i);
            seperator2.replace(0,strsection.length(),strsection);
            m_Text.append(seperator2);
            m_Text.append(seperator1);
        }
        /*Copy char * to QString.*/
        for(j=0; j<m_Size; j+=80)
        {
            baLine.clear();
            baLine.append(m_Data+j,80);
            baLine.append('\n');
            m_Text.append(baLine);
        }
    }
    /*Display in the texteditor.*/
    ui->textEdit->clear();
    ui->textEdit->setText(m_Text);
    if(m_TextHL!=NULL)
    {
        delete(m_TextHL);
        m_TextHL=NULL;
    }
    m_TextHL = new myHighLighter(ui->textEdit);
    m_TextHL->setMyHighLighterColor("END      ",Qt::yellow,Qt::blue);
    m_TextHL->setMyHighLighterColor("XTENSION ",Qt::yellow,Qt::blue);
    ui->textEdit->moveCursor(QTextCursor::Start);
    ui->label->setText(FilePath);
    ui->searchButton->setDisabled(false);
    return;
}


void fitsHeader::searchText()
{
    qDebug()<<"m_TextSD = "<<m_TextSD;
    if (!m_TextSD)
    {
        m_TextSD = new mySearchDialog(this);
        connect(m_TextSD, SIGNAL(searchingText(QString,QTextDocument::FindFlags,bool)),\
                this, SLOT(searchText(QString,QTextDocument::FindFlags,bool)));
    }
    m_TextSD->show();
    m_TextSD->raise();
    m_TextSD->activateWindow();
}

void fitsHeader::searchText(const QString &text, QTextDocument::FindFlags options, bool isReHighlighting)
{
    if(isReHighlighting)
    {
        if(m_TextHL!=NULL)
        {
            delete(m_TextHL);
            m_TextHL=NULL;
        }
        m_TextHL = new myHighLighter(ui->textEdit);
        ui->textEdit->setReadOnly(true);
        m_TextHL->setMyHighLighterColor(text,Qt::blue,Qt::yellow,options);
    }
    m_TextHL->setMyHighLighterColor("END     ",Qt::yellow,Qt::blue);
    ui->textEdit->find(text,options);
}

void fitsHeader::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
    QWidget::dragEnterEvent(event);
}


void fitsHeader::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    ui->textEdit->clear();
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

void fitsHeader::keyPressEvent(QKeyEvent *event)
{
    //
    QWidget::keyPressEvent(event);
}
