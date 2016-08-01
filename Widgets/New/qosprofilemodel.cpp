#include "qosprofilemodel.h"

QOSProfileModel::QOSProfileModel(QObject* parent):QAbstractItemModel(parent)
{

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
            if(nodeKind.endsWith("QOSProfile")){
                if(!viewItems.contains(ID)){
                    viewItem->registerObject(this);
                    viewItems[ID] = item;
                    profiles.append(ID);
                }
            }else if(profiles.contains(parentID)){
                if(!viewItems.contains(ID)){
                    viewItem->registerObject(this);
                    viewItems[ID] = item;
                    settings[parentID].append(ID);
                }
            }
        }
    }
}

void QOSProfileModel::viewItem_Destructed(int ID, ViewItem *viewItem)
{
    if(viewItems.contains(ID)){
        viewItems.remove(ID);
        if(profiles.contains(ID)){
            profiles.removeAll(ID);
        }
        if(settings.contains(ID)){
            profiles.removeAll(ID);
        }
        viewItem->unregisterObject(this);
    }
}

ViewItem *QOSProfileModel::getViewItem(const QModelIndex &index) const
{
    return 0;
}

int QOSProfileModel::rowCount(const QModelIndex &parent) const
{
    int column = parent.column();

    switch(column){
        case 0:
        case 1:
            return profiles.size();
        case 2:{
            ViewItem* item = getViewItem(parent);
            if(item && settings.contains(item->getID())){
                return settings[item->getID()].size();
            }
        }
        default:
            return 0;
    }
}

QVariant QOSProfileModel::data(const QModelIndex &index, int role) const
{
    ViewItem* item = getViewItem(index);

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        return "TEST";
    }
}

bool QOSProfileModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

QVariant QOSProfileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        if(section == 0){
            return "NAME";
        }else if(section == 1){
            return "MW";
        }else if(section == 2){
            return "Setting";
        }
    }
    return QVariant();
}

QModelIndex QOSProfileModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}

QModelIndex QOSProfileModel::parent(const QModelIndex &child) const
{

    return QModelIndex();
}

int QOSProfileModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

