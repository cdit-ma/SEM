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

    hiddenKeyNames << "width" << "height" <<  "x" << "y" << "isExpanded" << "originalID"; // << "kind";
    permanentlyLockedKeyNames << "kind";
 	multiLineKeyNames << "code";
    setupDataBinding();

}

AttributeTableModel::~AttributeTableModel()
{
    //qCritical() << "Deleting Table Model";
}

void AttributeTableModel::updatedData(GraphMLData *data)
{
    if(data){
        int ID = data->getID();
        int position = getIndex(ID);
        QModelIndex indexA = this->index(position, 0, QModelIndex());
        QModelIndex indexB = this->index(position, rowCount(indexA), QModelIndex());
        emit dataChanged(indexA, indexB);
    }
}

void AttributeTableModel::removedData(int dataID)
{
    //Get the Index of the data to be removed.
    int index = getIndex(dataID);
    if(index != -1){
        //Initiate the removal of the row.
        beginRemoveRows(QModelIndex(), index, index);

        //Remove it from the HashMap.
        attachedData.remove(dataID);

        //Get the DataName of the Key to be removed.
        QString deleteKey;
        foreach(QString key, nameLookup.keys()){
            if(nameLookup[key] == dataID){
                deleteKey = key;
                break;
            }
        }

        //Remove the DataName from the lookups.
        nameLookup.remove(deleteKey);
        dataOrder.removeAll(deleteKey);

        endRemoveRows();
    }
}

void AttributeTableModel::addData(GraphMLData *data)
{
    int ID = data->getID();

    //If we haven't seen this Data Before.
    if(!attachedData.contains(ID)){
        //If this data isn't meant to be hidden
        if(!hiddenKeyNames.contains(data->getKeyName())){
            //Stores the position we need to insert this Data to maintain ordering (LOCKED AT TOP, THEN ALPHA)
            int insertPosition = 0;

            //If the Data isn't permanently Locked, work out its position.
            if(!permanentlyLockedKeyNames.contains(data->getKeyName())){
                //If we don't find a spot, it must go at the end.
                insertPosition = dataOrder.size();


                for(int i=0; i < dataOrder.size(); i++){
                    QString currentKey = dataOrder[i];
                    //If the Data at i isn't permanently Locked, check if the data we are inserting belongs before it.
                    if(!permanentlyLockedKeyNames.contains(currentKey)){
                        if(data->getKeyName() < dataOrder[i]){
                            insertPosition = i;
                            break;
                       }
                    }
                }
            }

            connect(data, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));

            beginInsertRows(QModelIndex(), insertPosition, insertPosition);
            attachedData.insert(ID, data);
            dataOrder.insert(insertPosition, data->getKeyName());
            nameLookup.insert(data->getKeyName(), ID);
            updatedData(data);
            endInsertRows();

        }
    }
}

bool AttributeTableModel::hasData() const
{
    return !attachedData.isEmpty();
}

void AttributeTableModel::clearData()
{
    qCritical() << "LOST DATA";
    beginRemoveRows(QModelIndex(),0,attachedData.size());
    attachedData.clear();
    endRemoveRows();
}

int AttributeTableModel::getIndex(int ID) const
{



    return attachedData.keys().indexOf(ID);
}

GraphMLData* AttributeTableModel::getData(int row) const
{
    //GET DATA
    QString name = dataOrder[row];

    int ID = nameLookup[name];
    if(ID != -1){
        return attachedData[ID];
    }
    return 0;
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
    if(hasData()){
        return 3;
    }else{
        return 0;
    }
}

QVariant AttributeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    QString dataName =  dataOrder[row];

    int keyName = nameLookup[dataName];
    if(!attachedData.contains(keyName)){
        return QVariant();
    }

    if (index.row() >= attachedData.size() || index.row() < 0)
        return QVariant();

    // center align the Values column
    if (role == Qt::TextAlignmentRole) {
        switch(index.column()){

        case 1:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 2:{
            if(popupMultiLine(index)) {
                return QVariant(Qt::AlignLeft | Qt::AlignTop);
            }else{
                return QVariant(Qt::AlignCenter);

            }
        }
        }
    }

if (role == Qt::DecorationRole) {
        switch(index.column()){
        case 2:
            if(popupMultiLine(index)) {
                /*
                QImage* image = new QImage(":/Actions/popup.png");
                QImage scaledImage = image->scaled(15, 15, Qt::KeepAspectRatio);
                QPixmap pixmap(QPixmap::fromImage(scaledImage));
                return pixmap;*/
            }
           break;


        }
    }

    if (role == Qt::BackgroundRole){
        if(index.column() < 2){
         return QVariant::fromValue(QColor(240,240,240));
        }
    }





    if(role == Qt::CheckStateRole){
        GraphMLData* data = getData(index.row());
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
        GraphMLData* data = getData(index.row());

        switch(index.column()){
        case 0:
            return QVariant();
        case 1:
            return data->getKeyName();
        case 2:
            return data->getValue();
        default:
            return QVariant();
        }
    }

    if (role == Qt::EditRole) {
        GraphMLData* data  = getData(index.row());

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
        GraphMLData* data  = getData(index.row());

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
        GraphMLData* data  = getData(index.row());
        QVariant v(QMetaType::QObjectStar, &data);

        return v;
    }

 	//Role of -2 gives back a true/false value as to whether the multi-Line popup window should be visible or not
    if(role == -2) {
        return popupMultiLine(index);
    }

    if(role == -3) {
        GraphMLData* data = getData(index.row());
        if(data) {
            return data->getKeyName();
        }
    }
    return QVariant();

}

QVariant AttributeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!hasData()){
        return QVariant();
    }
    if (role == Qt::DecorationRole && orientation == Qt::Horizontal) {
        if(section == 0){
            QImage image(":/Actions/Lock_Closed.png");
            QImage scaledImage = image.scaled(24, 24, Qt::KeepAspectRatio);
            QPixmap pixmap(QPixmap::fromImage(scaledImage));
            return pixmap;

            //QImage image(":/")
            /*
            QImage* image = new QImage(":/Actions/lock.png");
            QImage scaledImage = image->scaled(15, 15, Qt::KeepAspectRatio);
            QPixmap pixmap(QPixmap::fromImage(scaledImage));
            return pixmap;
            */
        }
    }
     if (role == Qt::ToolTipRole) {
          if(section == 0){
              return QString("Locked Attributes");
          }
     }

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            break;

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
        GraphMLData* data  = getData(index.row());

        if(index.column() == 0 && data){
            if(!permanentlyLockedKeyNames.contains(data->getKeyName())){
                data->setProtected(value.toBool());
                dataChanged(index, index.sibling(index.row(),index.column()+2));
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
        GraphMLData* data  = getData(index.row());


        if (index.column() == 2 && data && !data->isProtected()){
            guiItem->GraphMLItem_TriggerAction("Updated Table Cell");

            guiItem->GraphMLItem_SetGraphMLData(guiItem->getID(), data->getKey()->getName(), value.toString());
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
    //bool allRemoved = true;
    bool anyRemoved = false;

    for (int row=0; row < rows; ++row) {
        GraphMLData* data = getData(position);
        if(data && !data->isProtected()){
            if(!anyRemoved){
                anyRemoved = true;
                guiItem->GraphMLItem_TriggerAction("Removed Table Cell");
            }
            guiItem->GraphMLItem_DestructGraphMLData(guiItem->getGraphML(), data->getKey()->getName());
        }else{
            //allRemoved = false;
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

            GraphMLData* data  = getData(row);
			if(!data->getParent()){
                return 0;
			}
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

            GraphMLData* data = getData(row);

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
        connect(attachedGraphML, SIGNAL(dataRemoved(int)), this, SLOT(removedData(int)));
        connect(attachedGraphML, SIGNAL(dataAdded(GraphMLData*)), this, SLOT(addData(GraphMLData*)));
        connect(attachedGraphML, SIGNAL(destroyed()), this, SLOT(clearData()));


        foreach(GraphMLData* data, attachedGraphML->getData()){
            if(!hiddenKeyNames.contains(data->getKeyName())){
                addData(data);
            }
        }
    }
}


/**
 * @brief AttributeTableModel::popupMultiLine
 * @param index
 * @return
 */
bool AttributeTableModel::popupMultiLine(const QModelIndex &index) const
{
    int row = index.row();
   QString dataName =  dataOrder[row];

   int keyName = nameLookup[dataName];
   if(!attachedData.contains(keyName)){
       return false;
   }

    GraphMLData* data = getData(index.row());
    if(data && index.column() == 2) {
        //Check types
        if(multiLineKeyNames.contains(data->getKeyName())) {
            //check if String
            if (data->getKey()->getType() == GraphMLKey::STRING) {
                return true;
            }
            //more types here :D
        }
    }
    return false;
}
