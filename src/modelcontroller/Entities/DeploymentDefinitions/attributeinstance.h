#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class AttributeInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	AttributeInstance(EntityFactory* factory);
	AttributeInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // ATTRIBUTEINSTANCE_H
