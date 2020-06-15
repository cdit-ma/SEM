#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class Attribute : public DataNode
{
    Q_OBJECT
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Attribute(EntityFactoryBroker& factory, bool is_temp_node);
    void parentSet(Node* parent);
};

#endif // ATTRIBUTE_H
