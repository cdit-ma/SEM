//
// Created by Cathlyn on 4/02/2020.
//

#include "triggeritemmodel.h"
#include "../../../modelcontroller/Entities/TriggerDefinitions/trigger.h"

/**
 * @brief TriggerItemModel::TriggerItemModel
 * @param parent
 */
TriggerItemModel::TriggerItemModel(QObject* parent)
        : QStandardItemModel(parent) {}


/**
 * @brief TriggerItemModel::addItemForTriggerDefinition
 * This constructs a model item connected to the given trigger_node_item and adds it to this model.
 * @param trigger_node_item
 * @return
 */
QModelIndex TriggerItemModel::addItemForTriggerDefinition(NodeViewItem& trigger_node_item)
{
    if (trigger_node_item.getNodeKind() == NODE_KIND::TRIGGER_DEFN) {
    
        // Remove data-fields that don't need to be shown in the data table for a Trigger definition
        auto item_data_model = trigger_node_item.getTableModel();
        item_data_model->removedData("kind");
        item_data_model->removedData("label");
        item_data_model->removedData("index");
        
        // Set custom data to link the trigger_node_item and to allow easy retrieval of the single-activation field
        auto model_item = new QStandardItem(trigger_node_item.getData("label").toString());
        model_item->setData(trigger_node_item.getID(), IDRole);
        model_item->setData(QVariant::fromValue(item_data_model), DataTableRole);
        
        auto&& wait_period_keyname = Trigger::getTableKeyString(Trigger::TableKey::WaitPeriod);
        model_item->setData(item_data_model->getIndex(wait_period_keyname), WaitPeriodRowRole);
        appendRow(model_item);
    
        // Connect the view item's dataChanged signal to the model item so that it can update its corresponding data accordingly
        connect(&trigger_node_item, &ViewItem::dataChanged, [model_item](const QString& key_name, const QVariant& data) {
            if (key_name == "label") {
                auto&& view_item_txt = data.toString();
                auto&& model_item_txt = model_item->text();
                // Only update the label if it has actually been changed
                if (model_item_txt != view_item_txt) {
                    model_item->setText(view_item_txt);
                }
            } else if (key_name == "single-activation") {
                // SingleActivationRole is used to tell the trigger browser whether to show/hide the wait period row
                model_item->setData(data.toBool(), SingleActivationRole);
            }
        });
    
        return model_item->index();
    }
    return QModelIndex();
}


/**
 * @brief TriggerItemModel::removeItemForTriggerDefinition
 * This searches for a model item whose IDRole data value matches trigger_definition_id and removes it from this model.
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
 * This returns the given model index's corresponding DataTableModel object.
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