#include "comboboxtabledelegate.h"
#include "attributetablemodel.h"
#include <QDebug>
#include <QComboBox>
#include "../../Model/data.h"
#include "../../Model/node.h"
ComboBoxTableDelegate::ComboBoxTableDelegate(QObject *parent):QItemDelegate(parent)
{

}

QWidget *ComboBoxTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStringList validList = getValidValueList(index);
    if(validList.size() > 0){
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(validList);

        return editor;
    }
    return QItemDelegate::createEditor(parent, option, index);
}

void ComboBoxTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStringList validList = getValidValueList(index);
    if(validList.size() > 0){
        QString currentValue = index.model()->data(index, Qt::EditRole).toString();

        QComboBox *spinBox = static_cast<QComboBox*>(editor);

        int i = spinBox->findText(currentValue);
        spinBox->setCurrentIndex(i);
        connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
        return;
    }
    QItemDelegate::setEditorData(editor,index);
}

void ComboBoxTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStringList validList = getValidValueList(index);
    if(validList.size() > 0){
        QComboBox *spinBox = static_cast<QComboBox*>(editor);
        QString value  = spinBox->currentText();
        model->setData(index, value, Qt::EditRole);
        return;
    }
    QItemDelegate::setModelData(editor, model, index);
}

void ComboBoxTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void ComboBoxTableDelegate::updateValue()
{
    QComboBox * cb = static_cast<QComboBox *>(sender());
    emit commitData(cb);
    emit closeEditor(cb);
}


EntityAdapter *ComboBoxTableDelegate::getEntityAdapter(const QModelIndex &index) const
{
    QObject* qObject = qvariant_cast<QObject *>(index.model()->data(index, -1));

    if(qObject){
        EntityAdapter * data = qobject_cast<EntityAdapter *>(qObject);
        if(data){
            return data;
        }

    }
    return 0;
}

QString ComboBoxTableDelegate::getKeyName(const QModelIndex &index) const
{
    return index.model()->data(index.model()->index(index.row(), 0)).toString();
}

QStringList ComboBoxTableDelegate::getValidValueList(const QModelIndex &index) const
{
    QStringList returnable;
    EntityAdapter* entityAdapter = getEntityAdapter(index);
    QString keyName = getKeyName(index);
    if(entityAdapter){
        returnable = entityAdapter->getValidValuesForKey(keyName);
    }
    return returnable;
}
