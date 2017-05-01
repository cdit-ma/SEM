#ifndef COMPONENTASSEMBLY_H
#define COMPONENTASSEMBLY_H
#include "../node.h"

class EntityFactory;
class ComponentAssembly : public Node
{
    Q_OBJECT
protected:
	ComponentAssembly(EntityFactory* factory);
	ComponentAssembly();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // COMPONENTASSEMBLY_H
