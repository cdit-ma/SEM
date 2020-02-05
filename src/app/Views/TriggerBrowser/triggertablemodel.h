//
// Created by Cathlyn on 3/02/2020.
//

#ifndef MEDEA_TRIGGERTABLEMODEL_H
#define MEDEA_TRIGGERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QSet>

class TriggerTableModel : public QAbstractTableModel
{
Q_OBJECT

public:
    enum TableRole {
        KeyRole = Qt::UserRole + 1
    };

    enum class TableKey {
        Type,
        Condition,
        Value,
        ReTrigger,
        WaitPeriod
    };

    explicit TriggerTableModel(QObject* parent = nullptr);

    bool reTriggerActive() const;

protected:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) final;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

private:
    const QSet<TableKey>& getTableKeys() const {
        static QSet<TableKey> trigger_table_keys ({
            TableKey::Type,
            TableKey::Condition,
            TableKey::Value,
            TableKey::ReTrigger,
            TableKey::WaitPeriod
        });
        return trigger_table_keys;
    };

    const QString& getTableKeyString(TableKey key) const {
        switch (key) {
            case TableKey::Type: {
                static QString tablekey_type_str = "type";
                return tablekey_type_str;
            }
            case TableKey::Condition: {
                static QString tablekey_condition_str = "condition";
                return tablekey_condition_str;
            }
            case TableKey::Value: {
                static QString tablekey_value_str = "value";
                return tablekey_value_str;
            }
            case TableKey::ReTrigger: {
                static QString tablekey_retrigger_str = "re-trigger?";
                return tablekey_retrigger_str;
            }
            case TableKey::WaitPeriod: {
                static QString tablekey_wait_period_str = "wait period (ms)";
                return tablekey_wait_period_str;
            }
            /*default: {
                static QString why = "Why?";
                return why;
            }*/
        }
    };

    int row_count_ = 4;
    QMap<QModelIndex, QVariant> data_map_;

};

inline uint qHash(TriggerTableModel::TableKey key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};

Q_DECLARE_METATYPE(TriggerTableModel::TableRole);
Q_DECLARE_METATYPE(TriggerTableModel::TableKey);

#endif //MEDEA_TRIGGERTABLEMODEL_H
