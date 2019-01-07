#ifndef TASKTREEMODELCLASS_H
#define TASKTREEMODELCLASS_H

/*
 * see http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html
 */

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVector>
#include <QDebug>
#include "TaskTreeItemClass.h"

class TaskTreeModelClass : public QAbstractItemModel
{
    Q_OBJECT

public:
    TaskTreeModelClass(const QStringList &headers, const QString &data, QObject *parent = nullptr);
    TaskTreeModelClass(QObject *parent = nullptr);
    ~TaskTreeModelClass() override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    bool insertColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;

    bool addItem(const QString &data, const QString &parentItemName = QString());
    TaskTreeItemClass *getItem(const QModelIndex &index) const;

private:
    void setupModelData(const QStringList &lines, TaskTreeItemClass *parent);
    TaskTreeItemClass *m_RootItem;
    QVector<TaskTreeItemClass*> m_Items;
    QVector<TaskTreeItemClass*> m_Parents;
    QVector<int> m_Indentations;
    QStringList m_ItemNames;

};

#endif // TASKTREEMODELCLASS_H
