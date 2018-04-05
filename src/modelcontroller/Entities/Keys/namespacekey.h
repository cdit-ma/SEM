#ifndef NAMESPACE_KEY_H
#define NAMESPACE_KEY_H
#include "../key.h"

class NamespaceKey : public Key
{
    Q_OBJECT
public:
    NamespaceKey();
    QVariant validateDataChange(Data* data, QVariant dataValue);
};


#endif //NAMESPACE_KEY_H
