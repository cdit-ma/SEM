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


Data *ComboBoxTableDelegate::getData(const QModelIndex &index) const
{
    QObject* qObject = qvariant_cast<QObject *>(index.model()->data(index, -1));

    if(qObject){
        Data * data = qobject_cast<Data *>(qObject);
        if(data){
            return data;
        }

    }
    return 0;
}

QStringList ComboBoxTableDelegate::getValidValueList(const QModelIndex &index) const
{
    QStringList returnable;
    Data* data = getData(index);
    if(data){
        QString nodeKind = "";

        Entity* dataParent = data->getParent();
        Key* key = data->getKey();

        if(dataParent){
            nodeKind = dataParent->getDataValue("kind").toString();
        }

        if(key && nodeKind != "" && key->gotValidValues(nodeKind)){
            returnable = key->getValidValues(nodeKind);
        }
    }
    return returnable;
}
