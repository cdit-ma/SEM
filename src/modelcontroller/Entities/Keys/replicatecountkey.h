#ifndef REPLICATE_COUNT_KEY_H
#define REPLICATE_COUNT_KEY_H
#include "../key.h"

class ReplicateCountKey : public Key
{
    Q_OBJECT
public:
    ReplicateCountKey(EntityFactory& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif // REPLICATE_COUNT_KEY_H
