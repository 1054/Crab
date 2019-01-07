#include "TaskTreeViewClass.h"

/*
 * overriding QTreeView class
 */


TaskTreeViewClass::TaskTreeViewClass(QWidget *parent)
: QTreeView(parent)
{
    this->setItemDelegateForColumn(0, new TaskTreeViewItemDelegateForTaskName(this));
    this->setItemDelegateForColumn(1, new TaskTreeViewItemDelegateForCheckBox(this));
}


TaskTreeViewClass::~TaskTreeViewClass()
{
}



void TaskTreeViewClass::mousePressEvent(QMouseEvent *event)
{
    //
    // deselect item when mouse pressed at empty position
    // see https://stackoverflow.com/questions/2761284/is-it-possible-to-deselect-in-a-qtreeview-by-clicking-off-an-item
    //
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    QTreeView::mousePressEvent(event);
    if ((item.row() == -1 && item.column() == -1) || selected) {
        clearSelection();
        const QModelIndex index;
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
    }

}


