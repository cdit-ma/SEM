#include "attributetablemodel.h"
#include "QDebug"
#include "nodeitem.h"
#include "nodeedge.h"

AttributeTableModel::AttributeTableModel(GraphMLItem *item, QObject *parent): QAbstractTableModel(item)
{
    Q_UNUSED(parent);
    guiItem = item;
    attachedGraphML = guiItem->getGraphML();
    setupDataBinding();
}

AttributeTableModel::~AttributeTableModel()
{
    //qCritical() << "Deleting Table Model";
}



void AttributeTableModel::updatedData(GraphMLData *data)
{
    int position = attachedData.indexOf(data);
    QModelIndex indexA = this->index(position, 0, QModelIndex());
    QModelIndex indexB = this->index(position, rowCount(indexA), QModelIndex());
    emit dataChanged(indexA, indexB);
}

void AttributeTableModel::removedData(GraphMLData *toRemove)
{
    //Remove the model index.
    int index = attachedData.indexOf(toRemove);
    if(index != -1){
        beginRemoveRows(QModelIndex(), index, index);
        attachedData.removeAt(index);
        endRemoveRows();
    }

}

void AttributeTableModel::addData(GraphMLData *data)
{
    if(!attachedData.contains(data)){
        beginInsertRows(QModelIndex(), attachedData.size(), attachedData.size());
        this->attachedData.append(data);
        updatedData(data);
        endInsertRows();
    }
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

        if (index.column() == 1 && data && !data->isProtected()){
            emit guiItem->triggerAction("Updated Table Cell");
            emit guiItem->updateGraphMLData(attachedGraphML, data->getKey()->getName(), value.toString());
            emit(dataChanged(index, index));
        }
    }

    return false;

}

bool AttributeTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row)
    Q_UNUSED(count)
    Q_UNUSED(parent)
    return true;
}

bool AttributeTableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    qCritical() << "Got to remove Data";
    Q_UNUSED(parent);
    bool allRemoved = true;

    bool anyRemoved = false;

    for (int row=0; row < rows; ++row) {
        GraphMLData* data = attachedData.at(position);
        if(data && !data->isProtected()){
            if(!anyRemoved){
                anyRemoved = true;
                emit guiItem->triggerAction("Removed Table Row");
            }
            emit guiItem->destructGraphMLData(attachedGraphML, data->getKey()->getName());
        }else{
            allRemoved = false;
        }

    }

    return true;

}

Qt::ItemFlags AttributeTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if(index.column() == 1){
        if(index.isValid()) {
            int row = index.row();

            GraphMLData* data = attachedData.at(row);

            if (data && data->isProtected()){
                return (QAbstractTableModel::flags(index)  ^ Qt::ItemIsEnabled);
            }else{
                return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled;
            }
        }
    }
    return Qt::ItemIsEnabled;

}

void AttributeTableModel::setupDataBinding()
{
    if(attachedGraphML){
        connect(attachedGraphML, SIGNAL(dataRemoved(GraphMLData*)), this, SLOT(removedData(GraphMLData*)));
        connect(attachedGraphML, SIGNAL(dataAdded(GraphMLData*)), this, SLOT(addData(GraphMLData*)));

        attachedData = attachedGraphML->getData();

        foreach(GraphMLData* data, attachedData){
            connect(data, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
        }
    }
}

