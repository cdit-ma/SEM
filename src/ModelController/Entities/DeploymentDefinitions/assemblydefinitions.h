#ifndef ASSEMBLYDEFINITIONS_H
#define ASSEMBLYDEFINITIONS_H
#include "../node.h"

#include "../../edgekinds.h"
class EntityFactory;
class AssemblyDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	AssemblyDefinitions(EntityFactory* factory);
	AssemblyDefinitions();
public:
    VIEW_ASPECT getViewAspect() const;
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};
#endif // AssemblyDefinitions_H
