#ifndef INNER_TYPE_KEY_H
#define INNER_TYPE_KEY_H
#include "../key.h"

class InnerTypeKey : public Key
{
    Q_OBJECT
public:
    InnerTypeKey();
    bool setData(Data* data, QVariant dataValue);
};

#endif //INNER_TYPE_KEY_H
