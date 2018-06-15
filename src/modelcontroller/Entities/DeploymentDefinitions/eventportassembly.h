#ifndef EVENTPORTDELEGATE_H
#define EVENTPORTDELEGATE_H
#include "../InterfaceDefinitions/eventport.h"


class EntityFactoryRegistryBroker;
class EventPortAssembly: public EventPort
{

    Q_OBJECT

protected:
    EventPortAssembly(EntityFactoryBroker& factory, NODE_KIND kind, bool is_temp, bool is_pubsub_port = true);
    void MiddlewareUpdated();

public:
    bool isPortDelegate() const;

    bool isPubPortDelegate() const;
    bool isSubPortDelegate() const;
    bool isReqPortDelegate() const;
    bool isPubSubPortDelegate() const;

    bool isSubPortAssembly() const;
    bool isPubPortAssembly() const;

    
    bool isPortInstance() const;

    bool isRepPortInstance() const;
    bool isReqPortInstance() const;
    bool isSubPortInstance() const;
    bool isPubPortInstance() const;

    

    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // EVENTPORTDELEGATE_H
