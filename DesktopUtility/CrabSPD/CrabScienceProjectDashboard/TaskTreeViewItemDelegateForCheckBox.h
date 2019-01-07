#ifndef TASKTREEVIEWITEMDELEGATEFORCHECKBOX_H
#define TASKTREEVIEWITEMDELEGATEFORCHECKBOX_H

/*
 * see https://forum.qt.io/topic/68542/custom-qitemdelegate-works-in-qtableview-but-not-qtreeview/8
 * see https://github.com/pierreet/BooleanCheckBoxDelegate/blob/master/BooleanCheckBoxDelegate.h
 */

/**************************************
* BooleanCheckBoxDelegate Class for Qt*
* Display checkbox instead of boolean *
* value in QTable class family        *
* Licence: Creatives Commons BY-NC-SA *
*    @author: Pierre-Etienne Crépy    *
*    @mail: pierre-etienne@crepy.eu   *
***************************************/

#include <QApplication>
#include <QStyledItemDelegate>
#include <QCheckBox>
#include <QPainter>
#include <QDebug>


class TaskTreeViewItemDelegateForCheckBox : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TaskTreeViewItemDelegateForCheckBox(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // TASKTREEVIEWITEMDELEGATEFORCHECKBOX_H
