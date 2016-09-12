#include "attributetablemodel.h"
#include "../viewitem.h"
#include "View/theme.h"
#include <QDebug>

AttributeTableModel::AttributeTableModel(ViewItem *item)
{
    entity = item;
    //Register the table
    entity->registerObject(this);

    multiLineKeys << "processes_to_log" << "code";
    //ignoredKeys << "x" << "y" << "width" << "height" << "isExpanded" << "readOnly";
    setupDataBinding();
}

AttributeTableModel::~AttributeTableModel()
{
    entity->unregisterObject(this);
}

void AttributeTableModel::updatedData(QString keyName)
{
    int row = getIndex(keyName);
    if(row > -1){
        QModelIndex indexA = index(row, 0, QModelIndex());
        QModelIndex indexB = index(row, rowCount(indexA), QModelIndex());
        emit dataChanged(indexA, indexB);
    }
}

void AttributeTableModel::removedData(QString keyName)
{
    //Get the Index of the data to be removed.
    int row = getIndex(keyName);
    if(row > -1){
        //Initiate the removal of the row.
        beginRemoveRows(QModelIndex(), row, row);

        //Remove it from the HashMap.
        editableKeys.removeAll(keyName);
        lockedKeys.removeAll(keyName);
        endRemoveRows();
    }
}

void AttributeTableModel::addData(QString keyName)
{
    //If we haven't seen this Data Before.

    if(editableKeys.contains(keyName) || lockedKeys.contains(keyName) || ignoredKeys.contains(keyName)){
        return;
    }

    //Ignore visual data
    if(entity->isDataVisual(keyName)){
        ignoredKeys.append(keyName);
        return;
    }

    bool locked = entity->isDataProtected(keyName);

    int insertIndex = 0;
    if(locked){
        insertIndex = lockedKeys.size();
        lockedKeys.append(keyName);
    }else{
        insertIndex = lockedKeys.size() + editableKeys.size();
        editableKeys.append(keyName);
    }

    //Insert Rows.
    beginInsertRows(QModelIndex(), insertIndex, insertIndex);
    endInsertRows();
    sort(0, Qt::AscendingOrder);
}

bool AttributeTableModel::hasData() const
{
    return rowCount(QModelIndex()) > 0;
}

void AttributeTableModel::clearData()
{
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()));
    editableKeys.clear();
    lockedKeys.clear();
    endRemoveRows();
}

int AttributeTableModel::getIndex(QString keyName) const
{
    int index = -1;
    if(lockedKeys.contains(keyName)){
        index = lockedKeys.indexOf(keyName);
    }else if(editableKeys.contains(keyName)){
        index = lockedKeys.size() + editableKeys.indexOf(keyName);
    }else if(ignoredKeys.contains(keyName)){
        index = -2;
    }
    return index;
}

QString AttributeTableModel::getKey(const QModelIndex &index) const
{
    return getKey(index.row());
}

QString AttributeTableModel::getKey(int row) const
{
    QString key;
    if(row < lockedKeys.size()){
        key = lockedKeys[row];
    }else{
        row -= lockedKeys.size();
        if(row < editableKeys.size()){
            key = editableKeys[row];
        }
    }
    return key;
}

bool AttributeTableModel::isIndexProtected(const QModelIndex &index) const
{
    return isRowProtected(index.row());
}

bool AttributeTableModel::isRowProtected(int row) const
{
    QString key = getKey(row);
    bool isProtected = true;
    if(entity){
        isProtected = entity->isDataProtected(key);
    }
    return isProtected;
}

bool AttributeTableModel::hasPopupEditor(const QModelIndex &index) const
{
    QString keyName = getKey(index);
    return multiLineKeys.contains(keyName);
}

QVariant AttributeTableModel::getData(const QModelIndex &index) const
{
    QVariant data;
    if(entity){
        QString key = getKey(index);
        data = entity->getData(key);
    }
    return data;
}




int AttributeTableModel::rowCount(const QModelIndex&) const
{
    return editableKeys.size() + lockedKeys.size();
}

int AttributeTableModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant AttributeTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        if(hasPopupEditor(index)){
            return QVariant(Qt::AlignLeft | Qt::AlignTop);
        }else{
            return QVariant(Qt::AlignCenter);
        }
    }
    if (role == Qt::DecorationRole) {
        if(hasPopupEditor(index)){
            return  Theme::theme()->getImage("Actions", "Popup", QSize(16,16), Theme::theme()->getAltBackgroundColor());
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        return getData(index);
    }

    if(role == MULTILINE_ROLE) {
        return hasPopupEditor(index);
    }

    if(role == VALID_VALUES_ROLE){
        if(entity){
            QString keyName = getKey(index);
            return entity->getValidValuesForKey(keyName);
        }
    }
    if(role == ID_ROLE){
        if(entity){
            return entity->getID();
        }
    }

    return QVariant();
}

QVariant AttributeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Vertical){
        return getKey(section);
    }
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        if(section == 0){
            return "Key";
        }else{
            return "Value";
        }
    }
    if (role == Qt::ToolTipRole) {
        if(isRowProtected(section)){
            return "Data is Protected";
        }
    }
    return QVariant();
}

bool AttributeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        int column = index.column();
        int row = index.row();
        if(column == 0 && !isIndexProtected(index)){
            QString key = getKey(row);
            emit req_dataChanged(entity->getID(), key, value);
            return true;
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
    if(index.isValid()){
        if(!isIndexProtected(index)){
            //Set it editable.
            return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }
    return Qt::NoItemFlags;
}

bool AttributeTableModel::isDataProtected(int row) const
{
    QString keyName = getKey(row);
    return entity->isDataProtected(keyName);
}


void AttributeTableModel::setupDataBinding()
{
    if(entity){
        //Attach each data.
        foreach(QString key, entity->getKeys()){
            addData(key);
        }

        connect(entity, SIGNAL(dataAdded(QString, QVariant)), this, SLOT(addData(QString)));
        connect(entity, SIGNAL(dataRemoved(QString)), this, SLOT(removedData(QString)));
        connect(entity, SIGNAL(dataChanged(QString, QVariant)), this, SLOT(updatedData(QString)));
        connect(entity, SIGNAL(destructing()), this, SLOT(deleteLater()));
    }
}



void AttributeTableModel::sort(int column, Qt::SortOrder order)
{
    if(order == Qt::AscendingOrder){
        qSort(lockedKeys.begin(), lockedKeys.end(), qLess<QString>());
        qSort(editableKeys.begin(), editableKeys.end(), qLess<QString>());
    }else{
        qSort(lockedKeys.begin(), lockedKeys.end(), qGreater<QString>());
        qSort(editableKeys.begin(), editableKeys.end(), qGreater<QString>());
    }

    QModelIndex indexA = index(0, 0, QModelIndex());
    QModelIndex indexB = index(columnCount(QModelIndex()), rowCount(indexA), QModelIndex());
    //Do some sorting!
    emit dataChanged(indexA, indexB);
}
