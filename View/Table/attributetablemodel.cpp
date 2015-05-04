#include "attributetablemodel.h"
#include "../GraphicsItems/nodeitem.h"
#include "../GraphicsItems/edgeitem.h"
#include <QGridLayout>
#include <QDebug>
#include <QDialog>
#include <QPlainTextEdit>
#include <QDialogButtonBox>

AttributeTableModel::AttributeTableModel(GraphMLItem *item, QObject *parent): QAbstractTableModel(item)
{
    Q_UNUSED(parent);
    guiItem = item;
    attachedGraphML = guiItem->getGraphML();
    if(attachedGraphML->isNode()){
        QString kind = attachedGraphML->getDataValue("kind");
        if(!(kind == "Aggregate" || kind == "AggregateInstance"
             || kind == "Member" || kind == "MemberInstance"
             || kind == "Condition" || kind == "Process")){
            hiddenKeyNames << "sortOrder";
        }
    }

    hiddenKeyNames << "width" << "height" <<  "x" << "y"; // << "kind";
    permanentlyLockedKeyNames << "kind";
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
        if(!hiddenKeyNames.contains(data->getKeyName())){
            connect(data, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
            beginInsertRows(QModelIndex(), attachedData.size(), attachedData.size());
            attachedData.append(data);
            updatedData(data);
            endInsertRows();
        }
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
    return 3;
}

QVariant AttributeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= attachedData.size() || index.row() < 0)
        return QVariant();

    // center align the Values column
    if (role == Qt::TextAlignmentRole) {
        switch(index.column()){
        case 2:
            return Qt::AlignCenter;
        }
    }

    if (role == Qt::DecorationRole) {
        GraphMLData* data = attachedData.at(index.row());
        switch(index.column()){
        case 0:
            if(data->isProtected()){
                QImage* image = new QImage(":/Resources/Icons/lock.png");
                QImage scaledImage = image->scaled(15, 15, Qt::KeepAspectRatio);
                QPixmap pixmap(QPixmap::fromImage(scaledImage));
                return pixmap;
            }else{
                QImage* image = new QImage(":/Resources/Icons/unlock.png");
                QImage scaledImage = image->scaled(15, 15, Qt::KeepAspectRatio);
                QPixmap pixmap(QPixmap::fromImage(scaledImage));
                return pixmap;
            }
            break;

        }
    }

    if(role == Qt::CheckStateRole){
        GraphMLData* data = attachedData.at(index.row());
        switch(index.column()){
        case 0:
            if(data->isProtected()){
                return Qt::Checked;
            }else{
               return Qt::Unchecked;
            }
        }
    }

    if (role == Qt::DisplayRole) {
        GraphMLData* data = attachedData.at(index.row());

        switch(index.column()){
        case 0:
            return QVariant();
            break;
        case 1:
            return data->getKey()->getName();
        case 2:
            return data->getValue();
        default:
            return QVariant();
        }
    }

    if (role == Qt::EditRole) {
        GraphMLData* data = attachedData.at(index.row());

        switch(index.column()){
        case 0:
            break;
        case 1:
            return data->getKeyName();
        case 2:
            return data->getValue();
        }
    }

    if (role == Qt::ToolTipRole) {
        GraphMLData* data = attachedData.at(index.row());

        switch(index.column()){
        case 0:{
            if(data->isProtected()){
                if(data->getParentData()){
                    return QString("Data is currently Locked and Bound to another GraphMLData.");
                }else{
                    return QString("Data is currently Locked.");
                }
            }else{
                return QString("Data is currrently Un-Locked.");
            }
        }
        case 1:
            return data->getKeyName();
        case 2:
            return data->getValue();
        }
    }
    if(role == -1){
        GraphMLData* data = attachedData.at(index.row());
        QVariant v(QMetaType::QObjectStar, &data);

        return v;
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
            return tr("");
        case 1:
            return tr("Key");
        case 2:
            return tr("Value");

        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool AttributeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::CheckStateRole){
        GraphMLData* data = attachedData.at(index.row());

        if(index.column() == 0 && data){
            if(!permanentlyLockedKeyNames.contains(data->getKeyName())){
                data->setProtected(value.toBool());
                dataChanged(index, index);
                return true;
            }
        }
/*
        switch(index.column()){
        case 0:
            if(data->isProtected()){
                return Qt::Checked;
            }else{
               return Qt::Unchecked;
            }

        }
        */
    }

    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        GraphMLData* data = attachedData.at(row);
/*
        if(index.column() == 0 && data){
            if(!permanentlyLockedKeyNames.contains(data->getKeyName())){
                data->setProtected(!data->isProtected());
                dataChanged(index, index);
                return true;
            }
        }
        */



        if (index.column() == 2 && data && !data->isProtected()){
            guiItem->GraphMLItem_TriggerAction("Updated Table Cell");        
            guiItem->GraphMLItem_SetGraphMLData(guiItem->getGraphML(), data->getKey()->getName(), value.toString());
            dataChanged(index, index);
            return true;
        }
    }

    /*
    if(role == Qt::CheckStateRole){
        GraphMLData* data = attachedData.at(index.row());
        switch(index.column()){
        case 0:
            if(data->isProtected()){
                return Qt::Checked;
            }else{
               return Qt::Unchecked;
            }
        }
    }
*/
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
                guiItem->GraphMLItem_TriggerAction("Removed Table Cell");
            }
            guiItem->GraphMLItem_DestructGraphMLData(guiItem->getGraphML(), data->getKey()->getName());
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
    if(index.column() == 0){
        if(index.isValid()) {
            int row = index.row();

            GraphMLData* data = attachedData.at(row);
            bool isEditable = data->getParentData() == 0;
            isEditable = !permanentlyLockedKeyNames.contains(data->getKeyName());

            if (data && isEditable){
                return QAbstractTableModel::flags(index) | /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
            }else{
                return (QAbstractTableModel::flags(index)  ^ Qt::ItemIsEnabled);
            }
        }
    }
    if(index.column() == 2){
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


        foreach(GraphMLData* data, attachedGraphML->getData()){
            addData(data);
        }
    }
}
