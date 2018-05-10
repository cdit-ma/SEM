#ifndef LABEL_KEY_H
#define LABEL_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class LabelKey : public Key
{
    Q_OBJECT
public:
    LabelKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif // KEY_H
