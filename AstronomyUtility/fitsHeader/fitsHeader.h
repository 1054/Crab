#ifndef FITSHEADER_H
#define FITSHEADER_H

#include <QWidget>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSyntaxHighlighter>
#include <QMimeData>
#include "myHighLighter.h"
#include "mySearchDialog.h"

namespace Ui {
    class fitsHeader;
}

class fitsHeader : public QWidget
{
    Q_OBJECT

public:
    explicit fitsHeader(QWidget *parent = 0);
    ~fitsHeader();
    int errorcode;
    bool    FileOpened;
    QString FilePath;
    QString FileType;
    QFileInfo FileInfo;

    void openFile(QString strFilePath);

public slots:
    void openFile();
    void searchText();
    void searchText(const QString &text, QTextDocument::FindFlags options=0, bool isReHighlighting=false);

private:
    Ui::fitsHeader *ui;
    char   *m_Data;
    int     m_Size;
    QString m_Text;
    myHighLighter *m_TextHL;
    mySearchDialog *m_TextSD;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void keyPressEvent(QKeyEvent *event);

};


#endif // FITSHEADER_H
