//
// Created by Cathlyn on 4/02/2020.
//

#ifndef MEDEA_TRIGGERITEMMODEL_H
#define MEDEA_TRIGGERITEMMODEL_H

#include "../../Controllers/ViewController/nodeviewitem.h"

#include <QStandardItemModel>

class TriggerItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit TriggerItemModel(QObject* parent = nullptr);
    
    enum TriggerItemRole {
        IDRole = Qt::UserRole + 1,
        DataTableRole = Qt::UserRole + 2,
        SingleActivationRole = Qt::UserRole + 3,
        WaitPeriodRowRole = Qt::UserRole + 4,
    };
    
    QModelIndex addItemForTriggerDefinition(NodeViewItem& trigger_node_item);
    bool removeItemForTriggerDefinition(int trigger_definition_id);
    
    DataTableModel* getTableModel(const QModelIndex& index) const;
};

#endif //MEDEA_TRIGGERITEMMODEL_H
