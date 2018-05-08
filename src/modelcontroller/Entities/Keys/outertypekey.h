#ifndef OUTER_TYPE_KEY_H
#define OUTER_TYPE_KEY_H
#include "../key.h"

class OuterTypeKey : public Key
{
    Q_OBJECT
public:
    OuterTypeKey(EntityFactory& factory);
    bool setData(Data* data, QVariant dataValue);
};

#endif //OUTER_TYPE_KEY_H
