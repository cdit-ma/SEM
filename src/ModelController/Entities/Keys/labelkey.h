#ifndef LABEL_KEY_H
#define LABEL_KEY_H
#include "../key.h"

class LabelKey : public Key
{
    Q_OBJECT
public:
    LabelKey();
    QVariant validateDataChange(Data* data, QVariant dataValue);
};

#endif // KEY_H
