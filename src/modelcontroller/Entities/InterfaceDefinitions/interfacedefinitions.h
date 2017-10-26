#ifndef INTERFACEDEFINITIONC_H
#define INTERFACEDEFINITIONC_H
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
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // INTERFACEDEFINITIONC_H
