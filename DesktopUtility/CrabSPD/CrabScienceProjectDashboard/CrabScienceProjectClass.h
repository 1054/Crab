#ifndef CRABSCIENCEPROJECTCLASS_H
#define CRABSCIENCEPROJECTCLASS_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "TaskTreeModelClass.h"

class CrabScienceProjectClass
{
public:
    CrabScienceProjectClass();
    QString RootPath;
    QString TaskFolderPath = QObject::tr("Task");
    QString TimeFolderPath = QObject::tr("Time");
    QString DataFolderPath = QObject::tr("Data");
    QString ToolFolderPath = QObject::tr("Tool");
    QString PublFolderPath = QObject::tr("Publ");

    TaskTreeModelClass* TaskTreeModel;

    void test_task_tree();

private:

};

#endif // CRABSCIENCEPROJECTCLASS_H
