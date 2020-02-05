//
// Created by Cathlyn on 3/02/2020.
//

#include "triggertablemodel.h"
#include <QDebug>

/**
 * @brief TriggerTableModel::TriggerTableModel
 * @param parent
 */
TriggerTableModel::TriggerTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    for (const auto& key : getTableKeys()) {
        auto row = static_cast<int>(key);
        setData(index(row, 0), QVariant::fromValue(key), TableRole::KeyRole);
    }
}


bool TriggerTableModel::reTriggerActive() const
{
    auto row = static_cast<int>(TableKey::ReTrigger);
    auto re_trigger_data = data(index(row, 0), Qt::DisplayRole);
    if (re_trigger_data.canConvert<bool>()) {
        return re_trigger_data.toBool();
    }
    return false;
}


/**
 * @brief TriggerTableModel::rowCount
 * @param parent
 * @return
 */
int TriggerTableModel::rowCount(const QModelIndex& parent) const
{
    // From Qt's API - "Note: When implementing a table based model, rowCount() should return 0 when the parent is valid."
    if (parent.isValid()) {
        return 0;
    }
    return getTableKeys().size();
}


/**
 * @brief TriggerTableModel::columnCount
 * @param parent
 * @return
 */
int TriggerTableModel::columnCount(const QModelIndex& parent) const
{
    // From Qt's API - "Note: When implementing a table based model, columnCount() should return 0 when the parent is valid."
    if (parent.isValid()) {
        return 0;
    }
    return 1;
}


/**
 * @brief TriggerTableModel::headerData
 * @param section
 * @param orientation
 * @param role
 * @return
 */
QVariant TriggerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return section + 1;
        } else {
            auto key = static_cast<TableKey>(section);
            return getTableKeyString(key);
        }
    }
    return QVariant();
}


/**
 * @brief TriggerTableModel::data
 * @param index
 * @param role
 * @return
 */
QVariant TriggerTableModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid()) {
        //return QAbstractTableModel::data(index, role);
        if (role == Qt::DisplayRole) {
            return data_map_.value(index, QVariant());
        }
    }
    return QVariant();
}


/**
 * @brief TriggerTableModel::setData
 * @param index
 * @param value
 * @param role
 * @return
 */
bool TriggerTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid()) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            data_map_.insert(index, value);
            //QAbstractTableModel::setData(index, value, role);
            emit dataChanged(index, index, QVector<int>{role});
            return true;
        }
    }
    return false;
}


/**
 * @brief TriggerTableModel
 * @param index
 * @return
 */
Qt::ItemFlags TriggerTableModel::flags(const QModelIndex& index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return Qt::NoItemFlags;
}


/**
 * @brief TriggerTableModel::insertRows
 * @param row
 * @param count
 * @param parent
 * @return
 */
bool TriggerTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || row > (row_count_ - 1)) {
        return false;
    }

    // begin/endInsertRows functions are required before and after removing the rows from the model
    beginInsertRows(parent, row, row + count - 1);
    QAbstractTableModel::insertRows(row, count, parent);
    endInsertRows();

    row_count_ += count;
    return true;
}


/**
 * @brief TriggerTableModel::removeRows
 * @param row
 * @param count
 * @param parent
 * @return
 */
bool TriggerTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (row < 0 || row > (row_count_ - 1)) {
        return false;
    }

    // begin/endRemoveRows functions are required before and after removing the rows from the model
    beginRemoveRows(parent, row, row + count - 1);
    QAbstractTableModel::removeRows(row, count, parent);
    endRemoveRows();

    row_count_ -= count;
    return true;
}