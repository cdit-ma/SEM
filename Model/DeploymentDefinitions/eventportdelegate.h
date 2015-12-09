#ifndef EVENTPORTDELEGATE_H
#define EVENTPORTDELEGATE_H
#include "../node.h"
#include "eventportinstance.h"
#include "../InterfaceDefinitions/aggregate.h"

class EventPortDelegate: public Node
{
    Q_OBJECT
public:
    EventPortDelegate(bool inEventPortDelegate);
    ~EventPortDelegate();

    bool isInEventPortDelegate();
    bool isOutEventPortDelegate();


    bool canAdoptChild(Node*);
    bool canConnect_AssemblyEdge(Node *node);
    bool canConnect_AggregateEdge(Node *aggregate);

    void setAggregate(Aggregate *aggregate);
    void unsetAggregate();
    Aggregate *getAggregate();
private:
    Aggregate* aggregate;
    bool inEventPortDelegate;
};

#endif // EVENTPORTDELEGATE_H
