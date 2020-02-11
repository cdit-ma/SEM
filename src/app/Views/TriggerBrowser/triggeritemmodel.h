//
// Created by Cathlyn on 4/02/2020.
//

#ifndef MEDEA_TRIGGERITEMMODEL_H
#define MEDEA_TRIGGERITEMMODEL_H

#include "triggertablemodel.h"
#include "../../Controllers/ViewController/nodeviewitem.h"

#include <QStandardItemModel>

class TriggerItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit TriggerItemModel(QObject* parent = nullptr);
    
    enum TriggerItemRole {
        IDRole = Qt::UserRole + 1,
        DataTableRole = Qt::UserRole + 2
    };
    
    QModelIndex addTriggerItemFor(NodeViewItem& node_item);
    DataTableModel* getTableModel(const QModelIndex& index) const;

private slots:
    void insertedRows(const QModelIndex& parent, int first, int last);
    void removedRows(const QModelIndex& parent, int first, int last);

private:
    QVector<TriggerTableModel*> table_models_;
};

#endif //MEDEA_TRIGGERITEMMODEL_H
