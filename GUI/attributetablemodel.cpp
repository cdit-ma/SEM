#include "attributetablemodel.h"
#include "QDebug"
#include "nodeitem.h"

AttributeTableModel::AttributeTableModel(NodeItem *item, QObject *parent): QAbstractTableModel(item)
{
    gui = item;
    attachedNode = item->node;
    attachedData = attachedNode->getData();
    foreach(GraphMLData* data, attachedData){
        connect(data, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    }

}



void AttributeTableModel::updatedData(GraphMLData *data)
{
    int position = attachedData.indexOf(data);
    QModelIndex indexA = this->index(position, 0, QModelIndex());
    QModelIndex indexB = this->index(position, rowCount(indexA), QModelIndex());

    emit dataChanged(indexA, indexB);

}


int AttributeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return attachedData.size();
}

int AttributeTableModel::columnCount(const QModelIndex &parent) const
{
    //Key Name, Data, For, Type
    Q_UNUSED(parent)
    return 2;
}

QVariant AttributeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= attachedData.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        GraphMLData* data = attachedData.at(index.row());

        switch(index.column()){
        case 0:
            return data->getKey()->getName();
        case 1:
            return data->getValue();
        default:
            return QVariant();
        }
    }
    return QVariant();

}

QVariant AttributeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Data Name");
        case 1:
            return tr("Data Value");

        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool AttributeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        GraphMLData* data = attachedData.at(row);

        if (index.column() == 1){
            if(value.toString() != ""){
                emit gui->actionTriggered("Updated Table Cell");
                emit gui->updateGraphMLData(attachedNode, data->getKey()->getName(), value.toString());
            }
            emit(dataChanged(index, index));
        }
    }

    return false;

}

bool AttributeTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return true;
}

bool AttributeTableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        GraphMLData* data = attachedData.at(position);
        attachedNode->removeData(data);
        attachedData.removeAt(position);
    }
    endRemoveRows();
    return true;

}

Qt::ItemFlags AttributeTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if(index.column() == 1){
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

}

