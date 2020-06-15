#ifndef LABEL_KEY_H
#define LABEL_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class LabelKey : public Key
{
    Q_OBJECT
public:
    LabelKey(EntityFactoryBroker& factory);
    static QVariant ValidateSystemLabel(Data* data, QVariant dataValue);

    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif // KEY_H
