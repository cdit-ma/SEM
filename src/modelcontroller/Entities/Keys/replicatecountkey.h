#ifndef REPLICATE_COUNT_KEY_H
#define REPLICATE_COUNT_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class ReplicateCountKey : public Key
{
    Q_OBJECT
public:
    ReplicateCountKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif // REPLICATE_COUNT_KEY_H
