#ifndef EVENTPORTDELEGATE_H
#define EVENTPORTDELEGATE_H
#include "../InterfaceDefinitions/eventport.h"

class EventPortAssembly: public EventPort
{
    Q_OBJECT
public:
    EventPortAssembly(NODE_KIND kind);

    bool isPortDelegate() const;

    bool isInPortDelegate() const;
    bool isOutPortDelegate() const;

    bool isInPortAssembly() const;
    bool isOutPortAssembly() const;


    bool isPortInstance() const;

    bool isInPortInstance() const;
    bool isOutPortInstance() const;

    virtual bool canAdoptChild(Node*);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // EVENTPORTDELEGATE_H
