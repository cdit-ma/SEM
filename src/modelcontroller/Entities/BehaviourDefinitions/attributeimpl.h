#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class AttributeImpl : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    AttributeImpl(EntityFactory& factory, bool is_temp_node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // ATTRIBUTEIMPL_H
