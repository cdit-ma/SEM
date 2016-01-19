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
    attachedEntity = guiItem->getEntityAdapter();
    if(attachedEntity->isNodeAdapter()){
        QString kind = attachedEntity->getDataValue("kind").toString();
        if(!(kind == "Aggregate" || kind == "AggregateInstance"
             || kind == "Member" || kind == "MemberInstance"
             || kind == "Condition" || kind == "Process" || kind =="Vector" || kind == "VectorInstance")){
            hiddenKeyNames << "sortOrder";
        }
    }

    attachedEntity->addListener(this);
    hiddenKeyNames << "width" << "height" <<  "x" << "y" << "originalID" << "isExpanded";//<< "kind";
    permanentlyLockedKeyNames << "kind";
 	multiLineKeyNames << "code";
    setupDataBinding();

}

AttributeTableModel::~AttributeTableModel()
{
    //qCritical() << "Deleting Table Model";
    attachedEntity->removeListener(this);
}

void AttributeTableModel::updatedData(QString keyName)
{
    int position = getIndex(keyName);
    if(position != -1){
        QModelIndex indexA = this->index(position, 0, QModelIndex());
        QModelIndex indexB = this->index(position, rowCount(indexA), QModelIndex());
        emit dataChanged(indexA, indexB);
    }
}

void AttributeTableModel::removedData(QString keyName)
{
    qCritical() << "REMOVED DATA";
    //Get the Index of the data to be removed.
    int index = getIndex(keyName);
    if(index != -1){
        //Initiate the removal of the row.
        beginRemoveRows(QModelIndex(), index, index);

        //Remove it from the HashMap.
        keys.removeAll(keyName);
        dataOrder.removeAll(keyName);

        endRemoveRows();
    }
}

void AttributeTableModel::addData(QString keyName)
{
    //If we haven't seen this Data Before.
    if(keys.contains(keyName) || hiddenKeyNames.contains(keyName)){
        return;
    }

    int insertPosition = 0;

    //If the Data isn't permanently Locked, work out its position.
    if(!permanentlyLockedKeyNames.contains(keyName)){
        insertPosition = dataOrder.size();
        for(int i=0; i < dataOrder.size(); i++){
            QString currentKey = dataOrder[i];
            //If the Data at i isn't permanently Locked, check if the data we are inserting belongs before it.
            if(!permanentlyLockedKeyNames.contains(currentKey)){
                if(keyName < currentKey){
                    insertPosition = i;
                    break;
               }
            }
        }
    }

    beginInsertRows(QModelIndex(), insertPosition, insertPosition);
    dataOrder.insert(insertPosition, keyName);
    keys.append(keyName);
    updatedData(keyName);
    endInsertRows();
}

bool AttributeTableModel::hasData() const
{
    return !dataOrder.isEmpty();
}

void AttributeTableModel::clearData()
{
    qCritical() << "CLEAR DATA";
    beginRemoveRows(QModelIndex(),0, dataOrder.size());
    dataOrder.clear();
    keys.clear();
    endRemoveRows();
}

int AttributeTableModel::getIndex(QString keyName) const
{
    return dataOrder.indexOf(keyName);
}

QVariant AttributeTableModel::getDataValue(int row) const
{
    QString name = dataOrder[row];
    return attachedEntity->getDataValue(name);
}

QString AttributeTableModel::getKeyName(int row) const
{
    return dataOrder[row];
}



int AttributeTableModel::rowCount(const QModelIndex&) const
{
    return dataOrder.size();
}

int AttributeTableModel::columnCount(const QModelIndex&) const
{
    if(hasData()){
        return 2;
    }
    return 0;
}

QVariant AttributeTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= dataOrder.size() || index.row() < 0)
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
                QImage* image = new QImage(":/Actions/Popup.png");
                QImage scaledImage = image->scaled(15, 15, Qt::KeepAspectRatio);
                QPixmap pixmap(QPixmap::fromImage(scaledImage));
                return pixmap;
            }
           break;
        }
    }

    if (role == Qt::BackgroundRole){
        if(index.column() < 1){
         return QVariant::fromValue(QColor(240,240,240));
        }
    }






    if (role == Qt::DisplayRole) {
        switch(index.column()){
        case 0:
            return getKeyName(index.row());
        case 1:
            return getDataValue(index.row());
        default:
            return QVariant();
        }
    }

    if (role == Qt::EditRole) {
        switch(index.column()){
        case 0:
            return getKeyName(index.row());
        case 1:
            return getDataValue(index.row());
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch(index.column()){
        case 0:
            return getKeyName(index.row());
        case 1:
            return getDataValue(index.row());
        }
    }
    if(role == -1){
        QVariant v(QMetaType::QObjectStar, &attachedEntity);
        return v;
        return 0;
    }

 	//Role of -2 gives back a true/false value as to whether the multi-Line popup window should be visible or not
    if(role == -2) {
        return popupMultiLine(index);
    }

    if(role == -3) {
        return getKeyName(index.row());
    }
    return QVariant();

}

QVariant AttributeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!hasData()){
        return QVariant();
    }
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "Key Name";
        case 1:
            return "Value";
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool AttributeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() == 1){
            if(!isDataProtected(index.row())){
                QString keyName = getKeyName(index.row());
                emit guiItem->GraphMLItem_TriggerAction("Setting Value for: " + keyName);
                guiItem->setData(keyName, value);
                return true;
            }
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


Qt::ItemFlags AttributeTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()){
        return Qt::ItemIsEnabled;
    }

    if(index.isValid()){
        if(index.column() == 1){
            if(!isDataProtected(index.row())){
                return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled;
            }
        }
    }

    return (QAbstractTableModel::flags(index)  ^ Qt::ItemIsEnabled);
}

bool AttributeTableModel::isDataProtected(int row) const
{
    QString keyName = dataOrder[row];
    return attachedEntity->isDataProtected(keyName);
}


void AttributeTableModel::setupDataBinding()
{
    if(attachedEntity){
        connect(attachedEntity, SIGNAL(dataAdded(QString, QVariant)), this, SLOT(addData(QString)));
        connect(attachedEntity, SIGNAL(dataRemoved(QString)), this, SLOT(removedData(QString)));
        connect(attachedEntity, SIGNAL(dataChanged(QString, QVariant)), this, SLOT(updatedData(QString)));

        connect(attachedEntity, SIGNAL(destroyed()), this, SLOT(clearData()));


        foreach(QString keyName, attachedEntity->getKeys()){
            if(!hiddenKeyNames.contains(keyName)){
                addData(keyName);
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
    if(index.column() == 2) {
        QString keyName = getKeyName(index.row());
        //Check types
        if(multiLineKeyNames.contains(keyName)) {
            //check if String
            if (attachedEntity->getKeyType(keyName) == QVariant::String) {
                return true;
            }
        }
    }
    return false;
}
