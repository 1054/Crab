#ifndef TASKTREEVIEWCLASS_H
#define TASKTREEVIEWCLASS_H

#include <QObject>
#include <QTreeView>
#include <QMouseEvent>
#include <QDebug>
#include "TaskTreeViewItemDelegateForCheckBox.h"
#include "TaskTreeViewItemDelegateForTaskName.h"

class TaskTreeViewClass : public QTreeView
{

public:
    TaskTreeViewClass(QWidget *parent = nullptr);
    ~TaskTreeViewClass() override;

private:
    void mousePressEvent(QMouseEvent *event) override;

};

#endif // TASKTREEVIEWCLASS_H
