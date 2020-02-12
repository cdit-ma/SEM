//
// Created by Cathlyn on 4/02/2020.
//

#include "triggeritemmodel.h"

/**
 * @brief TriggerItemModel::TriggerItemModel
 * @param parent
 */
TriggerItemModel::TriggerItemModel(QObject* parent)
        : QStandardItemModel(parent) {}


/**
 * @brief TriggerItemModel::addItemForTriggerDefinition
 * @param trigger_node_item
 * @return
 */
QModelIndex TriggerItemModel::addItemForTriggerDefinition(NodeViewItem& trigger_node_item)
{
    if (trigger_node_item.getNodeKind() == NODE_KIND::TRIGGER_DEFN) {
    
        auto&& item_data_model = trigger_node_item.getTableModel();
        
        // Set custom data for to link the trigger_node_item and to allow easy retrieval for the single-activation field
        auto model_item = new QStandardItem(trigger_node_item.getData("label").toString());
        model_item->setData(trigger_node_item.getID(), IDRole);
        model_item->setData(QVariant::fromValue(item_data_model), DataTableRole);
        model_item->setData(item_data_model->getIndex("wait-period (ms)"), WaitPeriodRowRole);
        appendRow(model_item);
    
        // Connect the view item's dataChanged to the model item so that their labels always match
        // SingleActivationRole is used to tell the trigger browser whether to show/hide the wait period row
        connect(&trigger_node_item, &ViewItem::dataChanged, [model_item](const QString& key_name, const QVariant& data) {
            // Only update the corresponding values if data has actually been changed
            if (key_name == "label") {
                auto&& view_item_txt = data.toString();
                auto&& model_item_txt = model_item->text();
                if (model_item_txt != view_item_txt) {
                    model_item->setText(view_item_txt);
                }
            } else if (key_name == "single-activation") {
                model_item->setData(data.toBool(), SingleActivationRole);
            }
        });
        
        return model_item->index();
    }
    return QModelIndex();
}


/**
 * @brief TriggerItemModel::removeItemForTriggerDefinition
 * @param trigger_definition_id
 * @return
 */
bool TriggerItemModel::removeItemForTriggerDefinition(int trigger_definition_id)
{
    for (int i = 0; i < rowCount(); i++) {
        auto&& trigger_item = item(i);
        auto&& item_id = trigger_item->data(IDRole).toInt();
        if (item_id == trigger_definition_id) {
            return removeRow(i);
        }
    }
    return false;
}


/**
 * @brief TriggerItemModel::getTableModel
 * This returns the provided model index's corresponding DataTableModel object
 * @param index
 * @return
 */
DataTableModel* TriggerItemModel::getTableModel(const QModelIndex& index) const
{
    if (index.isValid()) {
        return index.data(DataTableRole).value<DataTableModel*>();
    }
    return nullptr;
}