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
 * @brief TriggerItemModel::addTriggerItemFor
 * @param node_item
 * @return
 */
QModelIndex TriggerItemModel::addTriggerItemFor(NodeViewItem& node_item)
{
    if (node_item.getNodeKind() == NODE_KIND::TRIGGER_DEFN) {
    
        auto&& item_data_model = node_item.getTableModel();
        
        // Set custom data for to link the node_item and to allow easy retrieval for the single-activation field
        auto model_item = new QStandardItem(node_item.getData("label").toString());
        model_item->setData(node_item.getID(), IDRole);
        model_item->setData(QVariant::fromValue(item_data_model), DataTableRole);
        model_item->setData(false, SingleActivationRole);
        model_item->setData(item_data_model->getIndex("wait-period (ms)"), WaitPeriodRowRole);
        appendRow(model_item);
    
        // Connect the view to the model item so that their labels always match
        connect(&node_item, &ViewItem::dataChanged, [model_item](const QString& key_name, const QVariant& data) {
            /*
            // TODO - Might not need this because the Trigger entity isn't exposed in the view
            // Only update the corresponding values if data has actually been changed
            if (key_name == "label") {
                auto&& view_item_txt = data.toString();
                auto&& model_item_txt = model_item->text();
                if (model_item_txt != view_item_txt) {
                    model_item->setText(view_item_txt);
                }
            } else*/ if (key_name == "single-activation") {
                model_item->setData(data.toBool(), SingleActivationRole);
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
    }
    return nullptr;
}