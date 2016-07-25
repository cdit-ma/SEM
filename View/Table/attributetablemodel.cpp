#include "attributetablemodel.h"
#include "../GraphicsItems/nodeitem.h"
#include "../GraphicsItems/edgeitem.h"
#include <QGridLayout>
#include <QDebug>
#include <QDialog>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include "View/theme.h"
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

    attachedEntity->registerObject(this);
    hiddenKeyNames << "width" << "height" <<  "x" << "y" << "originalID" << "isExpanded" << "readOnly";//<< "kind";
    hiddenKeyNames << "snippetMAC" << "snippetTime" << "snippetID" << "exportTime" << "dataProtected";
    permanentlyLockedKeyNames << "kind";
    multiLineKeyNames << "code" << "processes_to_log";
    setupDataBinding();

}

AttributeTableModel::AttributeTableModel(EntityAdapter* adapter)
{
    guiItem = 0;
    attachedEntity = adapter;
    if(attachedEntity->isNodeAdapter()){
        QString kind = attachedEntity->getDataValue("kind").toString();
        if(!(kind == "Aggregate" || kind == "AggregateInstance"
             || kind == "Member" || kind == "MemberInstance"
             || kind == "Condition" || kind == "Process" || kind =="Vector" || kind == "VectorInstance")){
            hiddenKeyNames << "sortOrder";
        }
    }

    attachedEntity->registerObject(this);
    hiddenKeyNames << "width" << "height" <<  "x" << "y" << "originalID" << "isExpanded" << "readOnly";//<< "kind";
    hiddenKeyNames << "snippetMAC" << "snippetTime" << "snippetID" << "exportTime" << "dataProtected";
    permanentlyLockedKeyNames << "kind";
    multiLineKeyNames << "code" << "processes_to_log";
    setupDataBinding();
}

AttributeTableModel::~AttributeTableModel()
{
    attachedEntity->unregisterObject(this);
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
    return 1;
}

QVariant AttributeTableModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    int row = index.row();

    if (!index.isValid())
        return QVariant();

    if (index.row() >= dataOrder.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::TextAlignmentRole) {
        if(column == 0){
            if(popupMultiLine(index)) {
                return QVariant(Qt::AlignLeft | Qt::AlignTop);
            }else{
                return QVariant(Qt::AlignCenter);
            }
        }
    }

    if (role == Qt::DecorationRole) {
        if(column == 0){
            if(popupMultiLine(index)) {
                return  Theme::theme()->getImage("Actions", "Popup", QSize(16,16));
            }
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        if(column == 0){
            return getDataValue(row);
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
    if(role ==Qt::DisplayRole && orientation == Qt::Vertical){
        return getKeyName(section);
    }
    return QVariant();
}

bool AttributeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() == 0){
            if(!isDataProtected(index.row())){
                QString keyName = getKeyName(index.row());
                if (guiItem) {
                    emit guiItem->GraphMLItem_TriggerAction("Setting Value for: " + keyName);
                    guiItem->setData(keyName, value);
                }
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

    int row = index.row();
    int column = index.column();

    if(column == 0){
        bool isIndexProtected = isDataProtected(row);
        if(!isIndexProtected){
            //Set it editable.
            return  Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }

    return Qt::NoItemFlags;
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
    if(index.column() == 1) {
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
