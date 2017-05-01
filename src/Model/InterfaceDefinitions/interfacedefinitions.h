#ifndef INTERFACEDEFINITIONS_H
#define INTERFACEDEFINITIONS_H
#include "../node.h"

class EntityFactory;
class InterfaceDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InterfaceDefinitions(EntityFactory* factory);
	InterfaceDefinitions();
public:
    VIEW_ASPECT getViewAspect() const;

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // INTERFACEDEFINITIONS_H
