#ifndef WORKERDEFINITIONS_H
#define WORKERDEFINITIONS_H
#include "node.h"

class EntityFactory;
class WorkerDefinitions : public Node
{
    friend class EntityFactory;
    Q_OBJECT

protected:
	WorkerDefinitions(EntityFactory* factory);
	WorkerDefinitions();
public:
    VIEW_ASPECT getViewAspect() const;

    bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKERDEFINITIONS_H
