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


QModelIndex TriggerItemModel::addTriggerItemFor(NodeViewItem& node_item)
{
    if (node_item.getNodeKind() == NODE_KIND::TRIGGER_DEFN) {
    
        // Construct a new model_item and add it to the model
        auto model_item = new QStandardItem(node_item.getData("label").toString());
        model_item->setData(node_item.getID(), IDRole);
        model_item->setData(QVariant::fromValue(node_item.getTableModel()), DataTableRole);
        appendRow(model_item);
    
        // Connect the node_item's dataChanged signal to catch a change in label
        connect(&node_item, &ViewItem::dataChanged, [model_item](QString key_name, QVariant data) {
            // Only update the corresponding values if data has actually been changed
            if (key_name == "label") {
                auto&& view_item_txt = data.toString();
                auto&& model_item_txt = model_item->text();
                if (model_item_txt != view_item_txt) {
                    model_item->setText(view_item_txt);
                }
            }
        });
        return model_item->index();
    }
    return QModelIndex();
}


/**
 * @brief TriggerItemModel::getTableModel
 * This returns the provided model index's corresponding TriggerTableModel object.
 * @param index
 * @return
 */
DataTableModel* TriggerItemModel::getTableModel(const QModelIndex& index) const
{
    if (index.isValid()) {
        return index.data(DataTableRole).value<DataTableModel*>();
        /*
        auto&& row = index.row();
        if (row >= 0 && row < table_models_.size()) {
            return table_models_.at(row);
        }
        */
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