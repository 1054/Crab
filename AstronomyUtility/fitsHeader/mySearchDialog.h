#ifndef MYSEARCHDIALOG_H
#define MYSEARCHDIALOG_H

#include <QDialog>
#include <QTextDocument>
#include <QDebug>

namespace Ui {
    class mySearchDialog;
}

class mySearchDialog : public QDialog
{
    Q_OBJECT
public:
    mySearchDialog(QWidget *parent = 0);

signals:
    void searchingText(const QString &text, QTextDocument::FindFlags options=0, bool isReHighlighting=false);

public slots:
    void searchNext();
    void searchPrev();
    void searchChanged();

private:
    Ui::mySearchDialog *ui;
    bool isSearchChanged;

};

#endif // MYSEARCHDIALOG_H
