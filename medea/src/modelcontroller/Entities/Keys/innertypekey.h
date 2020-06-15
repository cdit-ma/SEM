#ifndef INNER_TYPE_KEY_H
#define INNER_TYPE_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class InnerTypeKey : public Key
{
    Q_OBJECT
public:
    InnerTypeKey(EntityFactoryBroker& factory);
    bool setData(Data* data, QVariant dataValue);
};

#endif //INNER_TYPE_KEY_H
