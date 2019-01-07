#ifndef TASKTREEITEMCLASS_H
#define TASKTREEITEMCLASS_H

/*
 * see http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html
 */

#include <QVariant>
#include <QVector>

class TaskTreeItemClass
{
    // see http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html

public:
    TaskTreeItemClass(const QVector<QVariant> &data, TaskTreeItemClass *parent = nullptr);
    ~TaskTreeItemClass();


    TaskTreeItemClass *parent();
    TaskTreeItemClass *child(int number);
    void appendChild(TaskTreeItemClass *child);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant &value);
    int row() const;
    bool insertColumns(int position, int columns);
    bool removeColumns(int position, int columns);
    bool insertChildren(int position, int count, int columns);
    bool removeChildren(int position, int count);
    int childNumber() const;

private:
    QVector<TaskTreeItemClass*> childItems;
    QVector<QVariant> itemData;
    TaskTreeItemClass *parentItem;
};

#endif // TASKTREEITEMCLASS_H
