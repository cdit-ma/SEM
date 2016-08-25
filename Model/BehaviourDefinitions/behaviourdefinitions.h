#ifndef BEHAVIOURDEFINITIONS_H
#define BEHAVIOURDEFINITIONS_H

#include "../node.h"

class BehaviourDefinitions: public Node
{
    Q_OBJECT
public:
    BehaviourDefinitions();
    VIEW_ASPECT getViewAspect() const;
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);

};

#endif // BEHAVIOURDEFINITIONS_H
