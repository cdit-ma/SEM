#ifndef EVENTPORT_H
#define EVENTPORT_H
#include "../node.h"

class Aggregate;

class EventPort : public Node
{
    Q_OBJECT

protected:
    EventPort(EntityFactoryBroker& factory, NODE_KIND kind, bool is_temp, bool is_pubsub_port=true);
public:
    bool isPubSubPort() const;
    bool isReqRepPort() const;

    void setPortType(Node* port_type);
    Node* getPortType();
    void unsetPortType();

    virtual bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);

private:
    Node* port_type = 0;
    bool is_pubsub_port = true;
};

#endif // EVENTPORT_H
