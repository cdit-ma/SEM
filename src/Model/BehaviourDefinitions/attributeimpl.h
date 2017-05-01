#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class AttributeImpl : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	AttributeImpl(EntityFactory* factory);
	AttributeImpl();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    QList<Data*> getDefaultData();
};

#endif // ATTRIBUTEIMPL_H
