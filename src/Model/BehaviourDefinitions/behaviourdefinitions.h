#ifndef BEHAVIOURDEFINITIONS_H
#define BEHAVIOURDEFINITIONS_H

#include "../node.h"

class EntityFactory;
class BehaviourDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	BehaviourDefinitions(EntityFactory* factory);
	BehaviourDefinitions();
public:
    VIEW_ASPECT getViewAspect() const;
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

};

#endif // BEHAVIOURDEFINITIONS_H
