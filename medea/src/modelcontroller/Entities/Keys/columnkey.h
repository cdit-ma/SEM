#ifndef COLUMN_KEY_H
#define COLUMN_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class ColumnKey : public Key
{
    Q_OBJECT
public:
    ColumnKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
    bool setData(Data* data, QVariant dataValue);
};

#endif //COLUMN_KEY_H
