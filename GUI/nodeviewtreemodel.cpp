#include "nodeviewtreemodel.h"
#include <QDebug>
NodeViewTreeModel::NodeViewTreeModel(QObject* parent): QAbstractItemModel(parent)
{
    rootItem = new NodeItemTreeItem();
}

NodeViewTreeModel::~NodeViewTreeModel()
{
    delete rootItem;
}

void NodeViewTreeModel::addToParentModel(NodeItemTreeItem *item)
{
    if(rootItem != 0){
        rootItem->appendChild(item);
        qCritical() << "ADDED!";
        qCritical() << rootItem->childCount();
    }
}

void NodeViewTreeModel::updatedData(GraphMLData *data)
{

}

Qt::ItemFlags NodeViewTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int NodeViewTreeModel::rowCount(const QModelIndex &parent) const
{
    NodeItemTreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<NodeItemTreeItem*>(parent.internalPointer());

    qCritical() << parentItem->childCount();
    return parentItem->childCount();

}

int NodeViewTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()){
        return static_cast<NodeItemTreeItem*>(parent.internalPointer())->columnCount();
    }else{
        return rootItem->columnCount();
    }

}

QVariant NodeViewTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    NodeItemTreeItem* item = static_cast<NodeItemTreeItem*>(index.internalPointer());
    qCritical() << "Row: " << index.row();
    qCritical() << "column: " << index.column();
    qCritical() << "Data: " << item->data(index.column());
    return item->data(index.column());
}

QVariant NodeViewTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();

}

QModelIndex NodeViewTreeModel::index(int row, int column, const QModelIndex &parent) const
{
      if (!hasIndex(row, column, parent))
        return QModelIndex();

    NodeItemTreeItem* parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<NodeItemTreeItem*>(parent.internalPointer());

    NodeItemTreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex NodeViewTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    NodeItemTreeItem* childItem = static_cast<NodeItemTreeItem*>(index.internalPointer());
    NodeItemTreeItem* parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

