#include "xmitreemodel.h"
#include <QDebug>
#include "../../View/theme.h"
#include <QStack>

XMITreeModel::XMITreeModel(QObject* parent):QStandardItemModel(parent)
{
}

QStringList XMITreeModel::getSelectedClassIDs()
{
    QStringList classIDs;
    foreach(QModelIndex root, getChildren()){
        QString type = root.data(XMI_TYPE).toString();
        QString ID = root.data(XMI_ID).toString();
        int selected = root.data(XMI_CHECKED).toInt();
        if(selected == Qt::Checked && type == "class"){
            classIDs.append(ID);
        }
    }
    return classIDs;
}

void XMITreeModel::selectAll(){
    foreach(QModelIndex root, getChildren(QModelIndex(), 0)){
        setData(root, Qt::Checked, XMI_CHECKED);
    }
}

void XMITreeModel::selectNone()
{
    foreach(QModelIndex root, getChildren(QModelIndex(), 0)){
        setData(root, Qt::Unchecked, XMI_CHECKED);
    }
}

QVariant XMITreeModel::data(const QModelIndex &index, int role) const
{
    if (role== Qt::DecorationRole)
    {
        QString type = data(index, XMI_TYPE).toString();
        QString iconName = "";
        if(type == "doc"){
            iconName = "Global";
        }else if(type == "package"){
            iconName = "Search_Folder";
        }else if(type == "class"){
            iconName = "GridLayout";
        }else if(type == "attribute"){
            iconName = "Tag";
        }
        return Theme::theme()->getImage("Actions", iconName, QSize(16,16));
    }else if(role == Qt::CheckStateRole){
        return data(index, XMI_CHECKED);
    }
    return QStandardItemModel::data(index, role);
}

bool XMITreeModel::setData(const QModelIndex &ind, const QVariant &value, int role)
{
    if(!ind.isValid()){
        return false;
    }

    if (role == Qt::CheckStateRole){
        return setData(ind, value, XMI_CHECKED);
    }else if(role == XMI_CHECKED){
        bool okay = QStandardItemModel::setData(ind, value, role);
        if(okay){
            foreach(QModelIndex descendant, getChildren(ind)){
                //Set all children's values to match our value.
                QStandardItemModel::setData(descendant, value, role);
            }

            QModelIndex topMostParent = ind;

            //Set parents
            QModelIndex parent = ind.parent();
            while(parent.isValid()){
                QVariant pV;
                //Check all descendants 1 deep to compare the values. If all are the same, set that as the parents value.
                //Else, set  Qt::PartiallyChecked
                foreach(QModelIndex d, getChildren(parent, 0)){
                    QVariant v = d.data(role);
                    if(!pV.isValid()){
                        pV = v;
                    }else if(pV != v){
                        pV = Qt::PartiallyChecked;
                        break;
                    }
                }
                QStandardItemModel::setData(parent, pV, role);
                //Step Up Parents
                if(!parent.parent().isValid()){
                    topMostParent = parent;
                }
                parent = parent.parent();
            }

            if(topMostParent.isValid()){
                int value = topMostParent.data(role).toInt();
                emit selectionChanged((Qt::CheckState)value);
            }
        }
        return okay;
    }

    return QStandardItemModel::setData(ind, value, role);
}

QList<QModelIndex> XMITreeModel::getChildren(const QModelIndex &ind, int depth) const
{
    QList<QModelIndex> descendants;
    QStack<QModelIndex> stack;
    stack.append(ind);

    while(!stack.isEmpty()){
        QModelIndex currentItem = stack.takeFirst();
        int maxX = rowCount(currentItem);
        int maxY = columnCount(currentItem);

        //Work out depth from ind
        int depthToInd = 0;
        QModelIndex parentItem = currentItem;
        while(parentItem != ind){
            depthToInd ++;
            parentItem = parentItem.parent();
        }
        if(depth == -1 || depthToInd <= depth){
            for(int x=0; x < maxX; x++){
                for(int y=0; y < maxY; y++){
                    QModelIndex child = index(x, y, currentItem);
                    if(child.isValid() && !descendants.contains(child)){
                        descendants.append(child);
                        stack.append(child);
                    }
                }
            }
        }
    }
    return descendants;
}

QVariant XMITreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return "UMI Element";
    }
    return QVariant();
}
