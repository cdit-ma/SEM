#ifndef EVENTPORTDELEGATE_H
#define EVENTPORTDELEGATE_H
#include "../InterfaceDefinitions/eventport.h"

class EntityFactory;
class EventPortAssembly: public EventPort
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	EventPortAssembly(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    EventPortAssembly(NODE_KIND kind);
public:

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
