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
    explicit TriggerTableModel(QObject* parent = nullptr);

    enum TableRole {
        KeyRole = Qt::UserRole + 1
    };

    enum class TableKey {
        Type,
        Condition,
        Value,
        SingleActivation,
        WaitPeriod
    };

    static const QStringList& getTriggerTypes() {
        static QStringList trigger_types({"CPU_util", "Mem_util", "temperature"});
        return trigger_types;
    }

    static const QStringList& getTriggerConditions() {
        static QStringList trigger_conditions({"<", "==", ">", "<=", ">=", "!="});
        return trigger_conditions;
    }

    bool singleActivation() const;

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
    static const QSet<TableKey>& getTableKeys() {
        static QSet<TableKey> trigger_table_keys({
            TableKey::Type,
            TableKey::Condition,
            TableKey::Value,
            TableKey::SingleActivation,
            TableKey::WaitPeriod
        });
        return trigger_table_keys;
    };

    static const QString& getTableKeyString(TableKey key) {
        switch (key) {
            case TableKey::Type: {
                static QString tablekey_type_str = "trigger-type";
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
            case TableKey::SingleActivation: {
                static QString tablekey_retrigger_str = "single-activation";
                return tablekey_retrigger_str;
            }
            case TableKey::WaitPeriod: {
                static QString tablekey_wait_period_str = "wait-period (ms)";
                return tablekey_wait_period_str;
            }
        }
    };

    int getTableKeyRow(TableKey key) const;

    QMap<QModelIndex, QPair<int, QVariant>> data_map_;
};

inline uint qHash(TriggerTableModel::TableKey key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};

Q_DECLARE_METATYPE(TriggerTableModel::TableRole);
Q_DECLARE_METATYPE(TriggerTableModel::TableKey);

#endif //MEDEA_TRIGGERTABLEMODEL_H
