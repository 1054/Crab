#ifndef CRABBLOBLISTWIDGET_H
#define CRABBLOBLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <CrabBlobItem.h>

class CrabBlobListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CrabBlobListWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
    QList<CrabBlobItem *> BlobList;
};

#endif // CRABBLOBLISTWIDGET_H
