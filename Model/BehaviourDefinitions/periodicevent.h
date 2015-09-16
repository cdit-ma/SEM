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
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // PERIODICEVENT_H

