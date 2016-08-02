#include "qosprofilemodel.h"

#include <QDebug>
#define ID_DATA -1
#include "../../View/theme.h"



QOSProfileModel::QOSProfileModel(QObject* parent):QStandardItemModel(parent)
{
    rootItem = invisibleRootItem();
}

void QOSProfileModel::viewItem_Constructed(ViewItem *viewItem)
{
    //We only care about NOdes.
    if(viewItem && viewItem->isNode()){
        int ID = viewItem->getID();
        NodeViewItem* item = (NodeViewItem*)viewItem;

        VIEW_ASPECT aspect = item->getViewAspect();
        if(aspect == VA_ASSEMBLIES){
            QString nodeKind = viewItem->getData("kind").toString();
            int parentID = item->getParentID();

            QStandardItem* parentItem = 0;
            if(nodeKind.endsWith("QOSProfile")){
                parentItem = invisibleRootItem();
            }else if(modelItems.contains(parentID)){
                parentItem = modelItems[parentID];
            }

            if(parentItem){
                QOSModelItem* modelItem = new QOSModelItem(item);
                parentItem->appendRow(modelItem);
                modelItems[ID] =  modelItem;
            }
        }
    }
}

void QOSProfileModel::viewItem_Destructed(int ID, ViewItem *viewItem)
{
    if(modelItems.contains(ID)){
        QOSModelItem* modelItem = modelItems[ID];
        modelItems.remove(ID);
        modelItem->deleteLater();
    }
}

QAbstractTableModel *QOSProfileModel::getTableModel(const QModelIndex &index) const
{
    if(index.isValid()){
        QStandardItem* item = itemFromIndex(index);
        if(item){
            return (QAbstractTableModel*) item->data(QOSProfileModel::DATATABLE_ROLE).value<void *>();
        }
    }
    return 0;
}

QVariant QOSProfileModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid()){
        QStandardItem* item = itemFromIndex(index);
        if(item){
            return item->data(role);
        }
    }
    return QStandardItemModel::data(index, role);
}

bool QOSProfileModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid()){
        QStandardItem* item = itemFromIndex(index);
        if(item){
            item->setData(value, role);
            return true;
        }
    }
    return false;
}


QOSModelItem::QOSModelItem(NodeViewItem *item):QObject(), QStandardItem()
{
    this->item = item;
    if(item){
        ID = item->getID();
        item->registerObject(this);
        connect(item, SIGNAL(iconChanged()), this, SLOT(itemChanged()));
        connect(item, SIGNAL(labelChanged(QString)), this, SLOT(itemChanged()));
    }else{
        ID = -1;
    }

}

NodeViewItem *QOSModelItem::getNodeViewItem()
{
    return item;
}

QOSModelItem::~QOSModelItem()
{
    if(item){
        item->unregisterObject(this);
    }
}

void QOSModelItem::itemChanged()
{
    emitDataChanged();
}

QVariant QOSModelItem::data(int role) const
{
    if(item){
        if(role == Qt::DisplayRole|| role == Qt::EditRole || role == Qt::ToolTipRole){
            return item->getData("label");
        }
        if(role == Qt::DecorationRole){
            return Theme::theme()->getIcon(item->getIcon());
        }
    }
    if(role == QOSProfileModel::ID_ROLE){
        return ID;
    }
    if(role == QOSProfileModel::DATATABLE_ROLE){
        if(item){
            return qVariantFromValue((void *)item->getTableModel());
        }else{
            return 0;
        }
    }
    return QStandardItem::data(role);
}

void QOSModelItem::setData(const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if(item && !item->isDataProtected("label")){
            emit item->getTableModel()->req_dataChanged(ID, "label", value);
        }
    }
}
