#ifndef CRABWORDSONTOP_H
#define CRABWORDSONTOP_H

#include <QWidget>
#include <QApplication>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QInputDialog>
#include <QDir>
#include <QString>
#include <QRegExp>
#include <QFontMetrics>
#include <QDebug>


namespace Ui {
class CrabWordsOnTop;
}

class CrabWordsOnTop : public QWidget
{
    Q_OBJECT

public:
    explicit CrabWordsOnTop(QWidget *parent = 0);
    ~CrabWordsOnTop();
    QString crab_note();

public slots:
    void crab_edit(QString input_text = "");
    void crab_move(qreal crab_move_to_x = -1.0, qreal crab_move_to_y = -1.0);

private:
    Ui::CrabWordsOnTop *ui;
};

#endif // CRABWORDSONTOP_H
