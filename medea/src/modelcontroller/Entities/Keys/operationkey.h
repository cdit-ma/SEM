#ifndef OPERATION_KEY_H
#define OPERATION_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class OperationKey : public Key
{
    Q_OBJECT
public:
    OperationKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif // OPERATION_KEY_H
