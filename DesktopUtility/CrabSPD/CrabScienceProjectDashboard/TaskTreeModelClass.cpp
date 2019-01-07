#include "TaskTreeModelClass.h"

/*
 * see http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html
 * and http://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-treemodel-cpp.html
 */


TaskTreeModelClass::TaskTreeModelClass(const QStringList &headers, const QString &data, QObject *parent)
: QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    foreach (QString header, headers) {
        rootData << header;
    }

    m_RootItem = new TaskTreeItemClass(rootData);
    setupModelData(data.split(QString("\n")), m_RootItem);

}



TaskTreeModelClass::TaskTreeModelClass(QObject *parent)
: QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    rootData << tr("Task Name");
    rootData << tr("Done");

    m_RootItem = new TaskTreeItemClass(rootData);

}



TaskTreeModelClass::~TaskTreeModelClass()
{
    delete m_RootItem;
}



QVariant TaskTreeModelClass::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_RootItem->data(section);

    return QVariant();
}



QVariant TaskTreeModelClass::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TaskTreeItemClass *item = static_cast<TaskTreeItemClass*>(index.internalPointer());

    return item->data(index.column());
}



QModelIndex TaskTreeModelClass::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0) {
        return QModelIndex();
    }

    TaskTreeItemClass *parentItem = getItem(parent);

    TaskTreeItemClass *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}



QModelIndex TaskTreeModelClass::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    TaskTreeItemClass *childItem = getItem(index);
    TaskTreeItemClass *parentItem = childItem->parent();

    if (parentItem == m_RootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->childNumber(), 0, parentItem);
}



int TaskTreeModelClass::rowCount(const QModelIndex &parent) const
{
    TaskTreeItemClass *parentItem = getItem(parent);

    return parentItem->childCount();
}



int TaskTreeModelClass::columnCount(const QModelIndex & /* parent */) const
{
    return m_RootItem->columnCount();
}



Qt::ItemFlags TaskTreeModelClass::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return nullptr;
    }

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}



bool TaskTreeModelClass::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TaskTreeItemClass *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}



bool TaskTreeModelClass::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = m_RootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}



bool TaskTreeModelClass::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = m_RootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}



bool TaskTreeModelClass::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = m_RootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (m_RootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}



bool TaskTreeModelClass::insertRows(int position, int rows, const QModelIndex &parent)
{
    TaskTreeItemClass *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, m_RootItem->columnCount());
    endInsertRows();

    return success;
}



bool TaskTreeModelClass::removeRows(int position, int rows, const QModelIndex &parent)
{
    TaskTreeItemClass *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}



void TaskTreeModelClass::setupModelData(const QStringList &lines, TaskTreeItemClass *parent)
{
    /*
     * setupModelData
     * each line in "lines" is an item,
     * if an item starts with whitespaces, then the number of whitespaces indicates its level
     * and if it contains \t, then \t separates multiple columns.
     */

    QVector<TaskTreeItemClass*> parents;
    QVector<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {

        QString lineData = lines[number];
        qDebug() << "TaskTreeModelClass::setupModelData()" << "line" << number << "/" << lines.count();

        // count leading white space
        int position = 0;
        while (position < lineData.length()) {
            if (lineData.at(position) != ' ') {
                break;
            }
            position++; // find first non-whitespace char, using whitespace to indicate levels
        }

        // trim leading white space
        lineData = lineData.mid(position).trimmed();

        // read lineData
        if (!lineData.isEmpty()) {
            // split lineData by \t and store into columnData
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QVector<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column) {
                columnData << columnStrings[column];
            }
            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.
                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            TaskTreeItemClass* parentNode = parents.last();
            TaskTreeItemClass* addedItem = new TaskTreeItemClass(columnData, parentNode);
            parentNode->appendChild(addedItem);
            m_ItemNames.append(columnStrings[0]);
            m_Items.append(addedItem);
            m_Parents.append(parents.last());
            m_Indentations.append(position);
        }

        ++number;
    }
}



TaskTreeItemClass *TaskTreeModelClass::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TaskTreeItemClass *item = static_cast<TaskTreeItemClass*>(index.internalPointer());
        if (item) {
            return item;
        }
    }
    return m_RootItem;
}



bool TaskTreeModelClass::addItem(const QString &data, const QString &parentItemName)
{
    if (!data.isEmpty()) {
        if (m_RootItem) {
            QString lineData = data;
            // count leading white space
            int position = 0;
            while (position < lineData.length()) {
                if (lineData.at(position) != ' ')
                    break;
                position++; // find first non-whitespace char, using whitespace to indicate levels
            }
            // trim leading white space
            lineData = lineData.mid(position).trimmed();
            // read lineData
            if (!lineData.isEmpty()) {
                // split lineData by \t and store into columnData
                QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
                QVector<QVariant> columnData;
                for (int column = 0; column < columnStrings.count(); ++column) {
                    columnData << columnStrings[column];
                }
                // determine a parent node where we can insert the new data as its child node.
                TaskTreeItemClass* parentNode = nullptr;
                if (m_Parents.size() == 0) {
                    // if the tree is empty (has headerData but no data), then just take the m_RootItem as the parent node.
                    parentNode = m_RootItem;
                } else {
                    // if the tree is not empty, then check whether parentItemName is given or not.
                    if (parentItemName.isEmpty()) {
                        // if parentItemName is not given, then append the new data to the last item with matched indentation
                        if (position > m_Indentations.last()) {
                            // if the new data has more indentation than the last item, then just append it as the child of the last item of the tree
                            parentNode = m_Items.last();
                        } else {
                            // otherwise find out the last item with matched indentation
                            int indexMatched = m_Items.size()-1;
                            while (indexMatched >= 0) {
                                if (m_Indentations[indexMatched] < position) {
                                    break;
                                }
                                indexMatched--;
                            }
                            if (indexMatched >= 0) {
                                parentNode = m_Items[indexMatched];
                            } else {
                                parentNode = m_RootItem;
                            }
                        }
                    } else {
                        // otherwise if parentItemName is given, then search for parentItemName
                        int indexMatched = m_Items.size()-1;
                        while (indexMatched >= 0) {
                            if (m_ItemNames[indexMatched] == parentItemName) {
                                break;
                            }
                            indexMatched--;
                        }
                        if (indexMatched >= 0) {
                            parentNode = m_Items[indexMatched];
                        } else {
                            // parentNode = m_RootItem;
                            qDebug() << "TaskTreeModelClass::addItem()" << "Error! parentItemName" << parentItemName << "not found in the tree!";
                            return false; // parentItemName not found!
                        }
                    }
                }

                if (parentNode) {
                    // Append a new item to the current parent's list of children.
                    TaskTreeItemClass* addedItem = new TaskTreeItemClass(columnData, parentNode);
                    parentNode->appendChild(addedItem);
                    m_ItemNames.append(columnStrings[0]);
                    m_Items.append(addedItem);
                    m_Parents.append(parentNode);
                    m_Indentations.append(position);

                    qDebug() << "TaskTreeModelClass::addItem()" << "added new item with columnData" << columnData;

                    // return true
                    return true;
                }
            }
        }
    }
    return false;
}



