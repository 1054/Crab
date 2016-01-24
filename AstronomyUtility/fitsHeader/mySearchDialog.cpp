#include "mySearchDialog.h"
#include "ui_mySearchDialog.h"

mySearchDialog::mySearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mySearchDialog)
{
    ui->setupUi(this);
    qDebug()<<"WindowFlags = "<<this->windowFlags();
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::Window|Qt::Dialog|Qt::WindowTitleHint);
    //Qt::WindowSystemMenuHint   Qt::WindowCloseButtonHint
    qDebug()<<"WindowFlags = "<<this->windowFlags();
    isSearchChanged=false;
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->searchNextButton,SIGNAL(clicked()),this,SLOT(searchNext()));
    connect(ui->searchPrevButton,SIGNAL(clicked()),this,SLOT(searchPrev()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(searchChanged()));
}

void mySearchDialog::searchNext()
{
    bool hl = false;
    qDebug()<<"isSearchChanged"<<isSearchChanged;
    if(isSearchChanged)
    {
        hl=true;
        isSearchChanged=false;
    }
    if(!ui->lineEdit->text().isEmpty())
    {
        if(ui->caseButton1->isChecked())
        {
            if(ui->wordButton1->isChecked())
                emit searchingText(ui->lineEdit->text(), 0, hl);
            //0x0 = forward + case insen + arbi
            else if(ui->wordButton2->isChecked())
                emit searchingText(ui->lineEdit->text(), QTextDocument::FindWholeWords, hl);
            //0x2 = forward + case insen + whole
        }
        else if(ui->caseButton2->isChecked())
        {
            if(ui->wordButton1->isChecked())
                emit searchingText(ui->lineEdit->text(), QTextDocument::FindCaseSensitively, hl);
            //0x4 = forward + case sensi + arbi
            else if(ui->wordButton2->isChecked())
                emit searchingText(ui->lineEdit->text(), \
                                   QTextDocument::FindCaseSensitively|QTextDocument::FindWholeWords, hl);
            //0x6 = forward + case sensi + whole
        }
    }
    //QTextDocument::           	 0x00000    find forward + case insensitively
    //QTextDocument::FindBackward	 0x00001
    //QTextDocument::FindCaseSensitively 0x00002
    //QTextDocument::FindWholeWords	 0x00004
}

void mySearchDialog::searchPrev()
{
    bool hl = false;
    qDebug()<<"isSearchChanged"<<isSearchChanged;
    if(isSearchChanged)
    {
        hl=true;
        isSearchChanged=false;
    }
    if(!ui->lineEdit->text().isEmpty())
    {
        if(ui->caseButton1->isChecked())
        {
            if(ui->wordButton1->isChecked())
                emit searchingText(ui->lineEdit->text(), QTextDocument::FindBackward, hl);
            //0x1 = backward + case insen + arbi
            else if(ui->wordButton2->isChecked())
                emit searchingText(ui->lineEdit->text(), \
                                   QTextDocument::FindBackward|QTextDocument::FindWholeWords, hl);
            //0x5 = backward + case insen + whole
        }
        else if(ui->caseButton2->isChecked())
        {
            if(ui->wordButton1->isChecked())
                emit searchingText(ui->lineEdit->text(), \
                                   QTextDocument::FindBackward|QTextDocument::FindCaseSensitively, hl);
            //0x3 = backward + case sensi + arbi
            else if(ui->wordButton2->isChecked())
                emit searchingText(ui->lineEdit->text(), \
QTextDocument::FindBackward|QTextDocument::FindCaseSensitively|QTextDocument::FindWholeWords, hl);
            //0x7 = backward + case sensi + whole
        }
    }
}

void mySearchDialog::searchChanged()
{
    isSearchChanged=true;
}
