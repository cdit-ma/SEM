//
// Created by Cathlyn on 6/02/2020.
//

#ifndef MEDEA_TRIGGER_H
#define MEDEA_TRIGGER_H

#include "../node.h"

class EntityFactoryRegistryBroker;
class Trigger : public Node
{
    Q_OBJECT
    
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker);
    
    enum class TableKey {
        Type,
        Condition,
        Value,
        SingleActivation,
        WaitPeriod
    };
    
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
    
    static const QStringList& getTriggerTypes() {
        static QStringList trigger_types({"CPU_util", "Mem_util", "temperature"});
        return trigger_types;
    }
    
    static const QStringList& getTriggerConditions() {
        static QStringList trigger_conditions({"<", "==", ">", "<=", ">=", "!="});
        return trigger_conditions;
    }
    
    VIEW_ASPECT getViewAspect() const final;
    
protected:
    Trigger(EntityFactoryBroker& factory_broker, bool is_temp_node);
};

inline uint qHash(Trigger::TableKey key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};

Q_DECLARE_METATYPE(Trigger::TableKey);

#endif //MEDEA_TRIGGER_H
