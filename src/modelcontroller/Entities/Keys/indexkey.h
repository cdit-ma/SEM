#ifndef INDEX_KEY_H
#define INDEX_KEY_H
#include "../key.h"
class EntityFactoryRegistryBroker;
class Node;
class EntityFactoryRegistryBroker;
class IndexKey : public Key
{
    Q_OBJECT
public:
    IndexKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);
    static void RevalidateChildrenIndex(Node* parent);
};

#endif //INDEX_KEY_H
