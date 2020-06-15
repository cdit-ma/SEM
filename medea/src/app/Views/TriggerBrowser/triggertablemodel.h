//
// Created by Cathlyn on 3/02/2020.
//

#ifndef MEDEA_TRIGGERTABLEMODEL_H
#define MEDEA_TRIGGERTABLEMODEL_H

#include "../../../modelcontroller/Entities/TriggerDefinitions/trigger.h"

#include <QAbstractTableModel>
#include <QSet>

class TriggerTableModel : public QAbstractTableModel
{
Q_OBJECT

public:
    explicit TriggerTableModel(QObject* parent = nullptr);

    enum TableRole {
        KeyRole = Qt::UserRole + 1
    };
    
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

protected:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) final;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

private:
    //int getTableKeyRow(Trigger::TableKey key) const;

    QMap<QModelIndex, QPair<int, QVariant>> data_map_;
};

Q_DECLARE_METATYPE(TriggerTableModel::TableRole);

#endif //MEDEA_TRIGGERTABLEMODEL_H
