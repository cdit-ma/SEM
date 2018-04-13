#ifndef ROW_KEY_H
#define ROW_KEY_H
#include "../key.h"

class RowKey : public Key
{
    Q_OBJECT
public:
    RowKey();
    QVariant validateDataChange(Data* data, QVariant dataValue);
    bool setData(Data* data, QVariant dataValue);
};

#endif //ROW_KEY_H
