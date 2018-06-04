#ifndef EVENTPORTDELEGATE_H
#define EVENTPORTDELEGATE_H
#include "../InterfaceDefinitions/eventport.h"


class EntityFactoryRegistryBroker;
class EventPortAssembly: public EventPort
{

    Q_OBJECT

protected:
    EventPortAssembly(EntityFactoryBroker& factory, NODE_KIND kind, bool is_temp);
    void MiddlewareUpdated();

public:
    bool isPortDelegate() const;

    bool isInPortDelegate() const;
    bool isOutPortDelegate() const;

    bool isInPortAssembly() const;
    bool isOutPortAssembly() const;

    
    bool isReplierPortInst() const;
    bool isRequesterPortInst() const;

    
    bool isPortInstance() const;

    bool isInPortInstance() const;
    bool isOutPortInstance() const;

    

    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // EVENTPORTDELEGATE_H
