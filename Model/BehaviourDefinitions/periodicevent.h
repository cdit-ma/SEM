#ifndef PERIODICEVENT_H
#define PERIODICEVENT_H
#include "behaviournode.h"

class PeriodicEvent: public BehaviourNode
{
    Q_OBJECT
public:
    PeriodicEvent();
    ~PeriodicEvent();
public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // PERIODICEVENT_H

