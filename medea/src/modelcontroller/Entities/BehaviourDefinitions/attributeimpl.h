#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "../InterfaceDefinitions/datanode.h"


class EntityFactoryRegistryBroker;
class AttributeImpl : public DataNode
{
    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    AttributeImpl(EntityFactoryBroker& factory, bool is_temp_node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // ATTRIBUTEIMPL_H
