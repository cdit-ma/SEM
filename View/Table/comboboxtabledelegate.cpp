#include "comboboxtabledelegate.h"
#include "attributetablemodel.h"
#include <QDebug>
#include <QComboBox>
#include "../../Model/graphmldata.h"
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
    editor->setGeometry(option.rect);
}

void ComboBoxTableDelegate::updateValue()
{
    QComboBox * cb = static_cast<QComboBox *>(sender());
    emit commitData(cb);
    emit closeEditor(cb);
}


GraphMLData *ComboBoxTableDelegate::getGraphMLData(const QModelIndex &index) const
{
    QObject* qObject = qvariant_cast<QObject *>(index.model()->data(index, -1));

    if(qObject){
        GraphMLData * data = qobject_cast<GraphMLData *>(qObject);
        if(data){
            return data;
        }

    }
    return 0;
}

QStringList ComboBoxTableDelegate::getValidValueList(const QModelIndex &index) const
{
    QStringList returnable;
    GraphMLData* data = getGraphMLData(index);
    if(data){
        QString nodeKind = "";

        GraphML* dataParent = data->getParent();
        GraphMLKey* key = data->getKey();

        if(dataParent){
            nodeKind = dataParent->getDataValue("kind");
        }

        if(key && nodeKind != "" && key->gotSelectableValues(nodeKind)){
            returnable = key->getSelectableValues(nodeKind);
        }
    }
    return returnable;
}
