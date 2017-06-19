#ifndef HARDWAREDEFINITIONC_H
#define HARDWAREDEFINITIONC_H

#include "../node.h"

class EntityFactory;
class HardwareDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	HardwareDefinitions(EntityFactory* factory);
	HardwareDefinitions();
public:
    VIEW_ASPECT getViewAspect() const;
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};
#endif // HARDWAREDEFINITIONC_H
