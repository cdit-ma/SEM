#ifndef INDEX_KEY_H
#define INDEX_KEY_H
#include "../key.h"

class IndexKey : public Key
{
    Q_OBJECT
public:
    IndexKey();
    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif //INDEX_KEY_H
