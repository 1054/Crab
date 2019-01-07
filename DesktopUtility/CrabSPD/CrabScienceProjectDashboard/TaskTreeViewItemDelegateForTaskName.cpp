#include "TaskTreeViewItemDelegateForTaskName.h"


TaskTreeViewItemDelegateForTaskName::TaskTreeViewItemDelegateForTaskName(QObject *parent)
: QStyledItemDelegate(parent)
{
}


QWidget *TaskTreeViewItemDelegateForTaskName::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    //qDebug() << "TaskTreeViewItemDelegateForTaskName::createEditor()" << "index.data()" << index.data();

    //create the checkbox editor
    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}


void TaskTreeViewItemDelegateForTaskName::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    qDebug() << "TaskTreeViewItemDelegateForTaskName::setEditorData()" << "index.data()" << index.data();
    //set if checked or not
    QLineEdit *editor2 = qobject_cast<QLineEdit *>(editor);
    editor2->setText(index.data().toString());
}


void TaskTreeViewItemDelegateForTaskName::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(editor);

    //QLineEdit *editor2 = static_cast<QLineEdit *>(editor);
    QString value = index.data().toString();
    model->setData(index, value, Qt::EditRole);
}


void TaskTreeViewItemDelegateForTaskName::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //retrieve data
    //QString data = index.model()->data(index, Qt::DisplayRole).toString();
    QStyledItemDelegate::paint(painter, option, index);
}


void TaskTreeViewItemDelegateForTaskName::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    //editor->setGeometry(QRect());

    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}
