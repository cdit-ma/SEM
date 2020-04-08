#include "datatablemodel.h"
#include "../../Controllers/ViewController/viewitem.h"
#include "../../theme.h"

#include <keynames.h>

const static QSet<QString> multiline_keys({KeyName::ProcessesToLog, KeyName::Code});
const static QSet<QString> icon_keys({KeyName::Icon, KeyName::IconPrefix});
const static QSet<QString> ignoredKeys({KeyName::X, KeyName::Y, KeyName::Width, KeyName::Height, KeyName::ReadOnly, KeyName::IsExpanded});

//TODO: FIX ME
/**
 * @brief DataTableModel::DataTableModel
 * @param item
 * @throws std::invalid_argument
 */
DataTableModel::DataTableModel(ViewItem *item)
{
	if (item == nullptr) {
		throw std::invalid_argument("DataTableModel::DataTableModel - view item cannot be null.");
	}

    entity = item;
    //Register the table
    entity->registerObject(this);

    setupDataBinding();
}

DataTableModel::~DataTableModel()
{
    entity->unregisterObject(this);
}

void DataTableModel::updatedData(const QString& keyName)
{
    int row = getIndex(keyName);
    if(row > -1){
        QModelIndex indexA = index(row, 0, QModelIndex());
        QModelIndex indexB = index(row, rowCount(indexA), QModelIndex());
        emit dataChanged(indexA, indexB);
    }
}

void DataTableModel::removedData(const QString& keyName)
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

void DataTableModel::addData(const QString& keyName)
{
    //If we haven't seen this Data Before.
    if(editableKeys.contains(keyName) || lockedKeys.contains(keyName) || ignoredKeys.contains(keyName)){
        return;
    }

    int insertIndex = lockedKeys.size() + editableKeys.size();
	editableKeys.append(keyName);

    //Insert Rows.
    beginInsertRows(QModelIndex(), insertIndex, insertIndex);
    endInsertRows();
    sort(0, Qt::AscendingOrder);
}

bool DataTableModel::hasData() const
{
    return rowCount(QModelIndex()) > 0;
}

void DataTableModel::clearData()
{
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()));
    editableKeys.clear();
    lockedKeys.clear();
    endRemoveRows();
}

int DataTableModel::getIndex(const QString& keyName) const
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

QString DataTableModel::getKey(const QModelIndex &index) const
{
    return getKey(index.row());
}

QString DataTableModel::getKey(int row) const
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

bool DataTableModel::isIndexProtected(const QModelIndex &index) const
{
    return isRowProtected(index.row());
}

bool DataTableModel::isRowProtected(int row) const
{
    QString key = getKey(row);
    bool isProtected = true;
    if(entity){
        isProtected = entity->isDataProtected(key);
    }
    return isProtected;
}

bool DataTableModel::hasCodeEditor(const QModelIndex &index) const
{
    QString key_name = getKey(index);
    return multiline_keys.contains(key_name);
}

bool DataTableModel::hasIconEditor(const QModelIndex &index) const
{
    QString key_name = getKey(index);
    return icon_keys.contains(key_name);
}

QVariant DataTableModel::getData(const QModelIndex &index) const
{
    QVariant data;
    if(entity){
        QString key = getKey(index);
        data = entity->getData(key);
    }
    return data;
}

int DataTableModel::rowCount(const QModelIndex&) const
{
    return editableKeys.size() + lockedKeys.size();
}

int DataTableModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
	switch (role) {
		case Qt::DisplayRole:
		case Qt::EditRole:
		case Qt::ToolTipRole:
			return getData(index);
		case Qt::TextAlignmentRole:
			if (hasCodeEditor(index)) {
				return QVariant(Qt::AlignLeft | Qt::AlignTop);
			} else {
				return QVariant(Qt::AlignCenter);
			}
		case Qt::DecorationRole:
			if (isIndexProtected(index)) {
				return Theme::theme()->getIcon("Icons", "lockClosed");
			}
			if (hasCodeEditor(index) || hasIconEditor(index)) {
				return Theme::theme()->getIcon("Icons", "popOut");
			}
			break;
		case MULTILINE_ROLE:
			return hasCodeEditor(index);
		case ICON_ROLE:
			return hasIconEditor(index);
		case VALID_VALUES_ROLE:
			if (entity) {
				return entity->getValidValuesForKey(getKey(index));
			}
			break;
		case ID_ROLE:
			if (entity) {
				return entity->getID();
			}
			break;
		default:
			break;
	}
	return QVariant();
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Vertical) {
			return getKey(section);
		} else {
			if (section == 0) {
				return "Key";
			} else {
				return "Value";
			}
		}
	} else if (role == Qt::ToolTipRole) {
		if (isRowProtected(section)) {
			return "Data is protected";
		}
	}
	// Uncomment for icons in the table
	/*else if (role == Qt::DecorationRole) {
		if (orientation == Qt::Vertical) {
			return  Theme::theme()->getImage("Data", getKey(section), QSize(16,16), Theme::theme()->getTextColor());
		}
	}*/
	return QVariant();
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        int column = index.column();
        int row = index.row();
        if(column == 0 && !isIndexProtected(index)){
            const auto& key = getKey(row);
            emit req_dataChanged(entity->getID(), key, value);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex &index) const
{
    if(index.isValid()){
        if(!isIndexProtected(index)){
            //Set it editable.
            return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }
    return Qt::NoItemFlags;
}

bool DataTableModel::isDataProtected(int row) const
{
    QString keyName = getKey(row);
    return entity->isDataProtected(keyName);
}

void DataTableModel::setupDataBinding()
{
    if (entity) {
        //Attach each data.
        auto key_list = entity->getKeys();
        std::sort(key_list.begin(), key_list.end());
        for (const auto& key : key_list) {
            addData(key);
        }
        connect(entity, &ViewItem::dataAdded, this, &DataTableModel::addData);
        connect(entity, &ViewItem::dataRemoved, this, &DataTableModel::removedData);
        connect(entity, &ViewItem::dataChanged, this, &DataTableModel::updatedData);
        connect(entity, &ViewItem::destructing, this, &DataTableModel::deleteLater);
    }
}

void DataTableModel::sort(int, Qt::SortOrder order)
{
	if (order == Qt::AscendingOrder) {
		std::sort(lockedKeys.begin(), lockedKeys.end(), std::less<QString>());
		std::sort(editableKeys.begin(), editableKeys.end(), std::less<QString>());
	} else {
		std::sort(lockedKeys.begin(), lockedKeys.end(), std::greater<QString>());
		std::sort(editableKeys.begin(), editableKeys.end(), std::greater<QString>());
	}

    QModelIndex indexA = index(0, 0, QModelIndex());
    QModelIndex indexB = index(columnCount(QModelIndex()), rowCount(indexA), QModelIndex());

    //Do some sorting!
    emit dataChanged(indexA, indexB);
}