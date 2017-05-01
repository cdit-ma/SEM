#ifndef ASSEMBLYDEFINITIONS_H
#define ASSEMBLYDEFINITIONS_H
#include "../node.h"

class EntityFactory;
class AssemblyDefinitions: public Node
{
    Q_OBJECT
protected:
	AssemblyDefinitions(EntityFactory* factory);
	AssemblyDefinitions();
public:
    VIEW_ASPECT getViewAspect() const;
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // AssemblyDefinitions_H
