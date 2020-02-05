//
// Created by Cathlyn on 4/02/2020.
//

#include "triggeritemmodel.h"

/**
 * @brief TriggerItemModel::TriggerItemModel
 * @param parent
 */
TriggerItemModel::TriggerItemModel(QObject* parent)
        : QStandardItemModel(parent)
{
    connect(this, &QStandardItemModel::rowsInserted, this, &TriggerItemModel::insertedRows);
    connect(this, &QStandardItemModel::rowsRemoved, this, &TriggerItemModel::removedRows);
}


/**
 * @brief TriggerItemModel::getTableModel
 * This returns the provided model index's corresponding TriggerTableModel object.
 * @param index
 * @return
 */
TriggerTableModel* TriggerItemModel::getTableModel(const QModelIndex& index) const
{
    if (index.isValid()) {
        auto&& row = index.row();
        if (row >= 0 && row < table_models_.size()) {
            return table_models_.at(row);
        }
    }
    return nullptr;
}


/**
 * @brief TriggerItemModel::insertedRows
 * This slot is called when rows have been inserted into this model i.e. when the rowsInserted signal has been emitted.
 * It constructs and stores a TriggerTableModel object for each inserted row.
 * @param parent
 * @param first
 * @param last
 */
void TriggerItemModel::insertedRows(const QModelIndex& parent, int first, int last)
{
    if (first < 0) {
        return;
    }
    for (int i = first; i <= last; i++) {
        table_models_.insert(i, new TriggerTableModel(this));
    }
}


/**
 * @brief TriggerItemModel::aboutToRemoveRows
 * This slot is called when rows have been removed from this model i.e. when the rowsRemoved signal has been emitted.
 * It removes the corresponding TriggerTableModel objects and then deletes them.
 * @param parent
 * @param first
 * @param last
 */
void TriggerItemModel::removedRows(const QModelIndex& parent, int first, int last)
{
    if (first < 0 || last >= table_models_.size()) {
        return;
    }
    for (int i = first; i <= last; i++) {
        auto table_model = table_models_.takeAt(i);
        table_model->deleteLater();
    }
}