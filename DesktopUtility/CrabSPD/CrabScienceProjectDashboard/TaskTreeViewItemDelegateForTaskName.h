#ifndef TASKTREEVIEWITEMDELEGATEFORTASKNAME_H
#define TASKTREEVIEWITEMDELEGATEFORTASKNAME_H

#include <QApplication>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QPainter>
#include <QDebug>


class TaskTreeViewItemDelegateForTaskName : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TaskTreeViewItemDelegateForTaskName(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // TASKTREEVIEWITEMDELEGATEFORTASKNAME_H
