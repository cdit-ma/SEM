#ifndef FREQUENCY_KEY_H
#define FREQUENCY_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class FrequencyKey : public Key
{
    Q_OBJECT
public:
    FrequencyKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif // FREQUENCY_KEY_H
