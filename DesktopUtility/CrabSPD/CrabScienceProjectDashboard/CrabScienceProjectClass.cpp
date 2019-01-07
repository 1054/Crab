#include "CrabScienceProjectClass.h"

CrabScienceProjectClass::CrabScienceProjectClass()
{
    // This is a class to store CrabScienceProject
    // A CrabScienceProject is a directory on disk with certain folder structures.


}



void CrabScienceProjectClass::test_task_tree()
{
    // test setting up a task tree and display it in UI "Task" tab
    QStringList headers;
    headers << QObject::tr("Title") << QObject::tr("Description");

    qDebug() << "CrabScienceProjectClass::test_task_tree() headers =" << headers;

    // test reading the data from a file
    //QFile file(":/test_task_tree.txt");
    //file.open(QIODevice::ReadOnly);
    //this->TaskTreeModel = new TaskTreeModelClass(headers, file.readAll());;
    //file.close();

    // test setting up the data
    /*
    QString data;
    QTextStream dataSteam(&data);
    dataSteam << "aaa" << "\t" << "bbb" << "\n";
    dataSteam << "    " << "\t" << "bbb2 ccc" << "\n";
    dataSteam << "        " << "\t" << "bbb3 ccc" << "\n";
    dataSteam << "         " << "\t" << "bbb4 ccc" << "\n";
    this->TaskTreeModel = new TaskTreeModelClass(headers, data);
    */

    this->TaskTreeModel = new TaskTreeModelClass();
    this->TaskTreeModel->addItem("task 1");
    this->TaskTreeModel->addItem("task 2");
    this->TaskTreeModel->addItem("sub task 1\ttrue\t", "task 2");
    this->TaskTreeModel->addItem("sub task 2\ttrue\t", "task 2");
    this->TaskTreeModel->addItem("sub task 1\tfalse\t", "task 1");
    this->TaskTreeModel->addItem("sub task 2\tfalse\t", "task 1");

    //qDebug() << "CrabScienceProjectClass::test_task_tree() data =" << data;

}



