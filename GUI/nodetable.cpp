#include "NodeTable.h"
#include "graphmlview.h"
#include <QDebug>
#include <QVariant>
NodeTable::NodeTable():QAbstractTableModel()
{
    qCritical() << "Constructed NodeTable";
    rootItem = 0;
}

void NodeTable::addItem(Node *node)
{

    if(node){
        emit layoutAboutToBeChanged();



        NodeTableItem* parentItem = 0;

        if(node->getParentNode()){
            QString parentID = node->getParentNode()->getID();
            parentItem = modelLookup[parentID];
        }


        NodeTableItem*  tableItem = new NodeTableItem(node, parentItem);

        if(!node->getParentNode()){
            qCritical() << "Making Root Item";
            rootItem = tableItem;
        }

        GraphMLData* label = node->getData("label");
        connect(label, SIGNAL(dataChanged(GraphMLData*)), this, SLOT(updatedData(GraphMLData*)));

        storeNodeInHash(tableItem);


        //emit rowsInserted();

        //rowsInserted(parentItem,0,1,QPrivateSignal::QPrivateSignal);
        //QModelIndex indexA = this->index(0, 0, QModelIndex());
       // QModelIndex indexB = this->index(rowCount(QModelIndex()), columnCount(QModelIndex()), QModelIndex());
        //emit dataChanged(indexA,indexB);

        emit layoutChanged();
    }

}

void NodeTable::removeItem(QString ID)
{

    emit layoutAboutToBeChanged();
    removeNodeFromHash(ID);
    emit layoutChanged();
}

Node *NodeTable::getNodeFromIndex(QModelIndex index)
{
    if (index.isValid()){
        NodeTableItem *item = static_cast<NodeTableItem*>(index.internalPointer());
        return item->getNode();
    }

    return 0;
}


void NodeTable::updatedData(GraphMLData *data)
{

    if(data->getKeyName() == "label"){
        QModelIndex indexA = this->index(0, 0, QModelIndex());
        QModelIndex indexB = this->index(rowCount(QModelIndex()), columnCount(QModelIndex()), QModelIndex());
        emit dataChanged(indexA,indexB);
    }


}


int NodeTable::rowCount(const QModelIndex &parent) const
{
    NodeTableItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<NodeTableItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int NodeTable::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
            return static_cast<NodeTableItem*>(parent.internalPointer())->columnCount();
        else
            return rootItem->columnCount();
}

QVariant NodeTable::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QVariant();
    }

    if(role == Qt::DecorationRole){
        if(index.column() == 0){
        NodeTableItem *item = static_cast<NodeTableItem*>(index.internalPointer());
        QString kind = item->getNode()->getDataValue("kind");
        return QIcon(":/Resources/Icons/" + kind + ".png");
        }
    }

    if (role != Qt::DisplayRole){
        return QVariant();
    }

    NodeTableItem *item = static_cast<NodeTableItem*>(index.internalPointer());

    return item->data(index.column());
}

bool NodeTable::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        NodeTableItem *item = static_cast<NodeTableItem*>(index.internalPointer());

        Node* node = item->getNode();

        emit triggerAction("Updated Table Cell");
        emit updateGraphMLData(node, "label", value.toString());
    }
    return false;

}

QVariant NodeTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Node Kind");
        case 1:
            return tr("Node Label");
        case 2:
            return tr("Node Value");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags NodeTable::flags(const QModelIndex &index) const
{
    if (!index.isValid()){
        return 0;
    }

    if(index.column() == 0){
        NodeTableItem *item = static_cast<NodeTableItem*>(index.internalPointer());
        if(item){
            GraphMLData* labelData = item->getNode()->getData("label");
            if(labelData && labelData->isProtected()){
                return  !Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            }else{
                return  Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;

            }
        }
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void NodeTable::storeNodeInHash(NodeTableItem *item)
{
    Node* node = item->getNode();
    if(node){
        QString ID = node->getID();
        if(modelLookup.contains(ID)){
            qCritical() << "Hash already contains GraphMLItem with ID: " << ID;
        }else{
            modelLookup[ID] = item;
        }
    }

}

bool NodeTable::removeNodeFromHash(QString ID)
{
    if(modelLookup.contains(ID)){
        //qCritical() << "Hash Removed ID: " << ID;
        NodeTableItem* item = modelLookup[ID];

        modelLookup.remove(ID);

        if(item){
            qCritical() << "Deleting Item";
            delete item;
            return true;
        }
    }else{
        qCritical() << "Not HERE?!" << ID;
    }
    return false;
}

QModelIndex NodeTable::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    NodeTableItem *parentItem = 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<NodeTableItem*>(parent.internalPointer());

    NodeTableItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}

QModelIndex NodeTable::parent(const QModelIndex &index) const
{
    if (!index.isValid())
            return QModelIndex();

    NodeTableItem *childItem = static_cast<NodeTableItem*>(index.internalPointer());
    NodeTableItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);

}
