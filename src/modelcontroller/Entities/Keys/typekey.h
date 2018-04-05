#ifndef TYPE_KEY_H
#define TYPE_KEY_H
#include "../key.h"

class Node;

class TypeKey : public Key
{
    Q_OBJECT
public:
    TypeKey();
    QVariant validateDataChange(Data* data, QVariant dataValue);

    static bool BindTypes(Node* src, Node* dst, bool bind);
};

#endif //TYPE_KEY_H
