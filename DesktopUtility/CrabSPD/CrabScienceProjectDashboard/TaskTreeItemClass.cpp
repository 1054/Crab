#include "TaskTreeItemClass.h"

/*
 * see http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html
 */


TaskTreeItemClass::TaskTreeItemClass(const QVector<QVariant> &data, TaskTreeItemClass *parent)
{
    parentItem = parent;
    itemData = data;
}


TaskTreeItemClass::~TaskTreeItemClass()
{
    qDeleteAll(childItems);
}


TaskTreeItemClass *TaskTreeItemClass::parent()
{
    return parentItem;
}


TaskTreeItemClass *TaskTreeItemClass::child(int number)
{
    // child() returns a specific child from the internal list of children:
    return childItems.value(number);
}


void TaskTreeItemClass::appendChild(TaskTreeItemClass *item)
{
    childItems.append(item);
}


int TaskTreeItemClass::childCount() const
{
    // The childCount() function returns the total number of children:
    // see http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html
    return childItems.count();
}


int TaskTreeItemClass::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TaskTreeItemClass*>(this));

    return 0;
}


int TaskTreeItemClass::columnCount() const
{
    return itemData.count();
}


QVariant TaskTreeItemClass::data(int column) const
{
    return itemData.value(column);
}


bool TaskTreeItemClass::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}


int TaskTreeItemClass::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TaskTreeItemClass*>(this));

    return 0;
}


bool TaskTreeItemClass::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    foreach (TaskTreeItemClass *child, childItems)
        child->insertColumns(position, columns);

    return true;
}


bool TaskTreeItemClass::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (TaskTreeItemClass *child, childItems)
        child->removeColumns(position, columns);

    return true;
}


bool TaskTreeItemClass::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        TaskTreeItemClass *item = new TaskTreeItemClass(data, this);
        childItems.insert(position, item);
    }

    return true;
}


bool TaskTreeItemClass::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}




