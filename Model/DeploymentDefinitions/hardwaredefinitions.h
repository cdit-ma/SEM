#ifndef HARDWAREDEFINITIONS_H
#define HARDWAREDEFINITIONS_H

#include "../node.h"

class HardwareDefinitions: public Node
{
    Q_OBJECT
public:
    HardwareDefinitions();
    VIEW_ASPECT getViewAspect() const;
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // HARDWAREDEFINITIONS_H
