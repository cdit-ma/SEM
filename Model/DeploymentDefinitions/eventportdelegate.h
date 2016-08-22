#ifndef EVENTPORTDELEGATE_H
#define EVENTPORTDELEGATE_H
#include "../InterfaceDefinitions/eventport.h"

class EventPortAssembly: public EventPort
{
    Q_OBJECT
public:
    EventPortAssembly(Node::NODE_KIND kind);

    bool isPortDelegate() const;

    bool isInPortDelegate() const;
    bool isOutPortDelegate() const;


    bool isPortInstance() const;

    bool isInPortInstance() const;
    bool isOutPortInstance() const;

    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // EVENTPORTDELEGATE_H
