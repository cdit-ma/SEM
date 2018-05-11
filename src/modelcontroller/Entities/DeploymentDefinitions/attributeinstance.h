#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../InterfaceDefinitions/datanode.h"


class EntityFactoryRegistryBroker;
class AttributeInstance : public DataNode
{

    Q_OBJECT

public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    AttributeInstance(EntityFactoryBroker& factory, bool is_temp_node);
    void parentSet(Node* parent);
};

#endif // ATTRIBUTEINSTANCE_H
