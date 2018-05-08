#ifndef INDEX_KEY_H
#define INDEX_KEY_H
#include "../key.h"
class Node;
class IndexKey : public Key
{
    Q_OBJECT
public:
    IndexKey(EntityFactory& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
    static void RevalidateChildrenIndex(Node* parent);
};

#endif //INDEX_KEY_H
