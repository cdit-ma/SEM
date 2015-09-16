#ifndef WORKLOAD_H
#define WORKLOAD_H

#include "behaviournode.h"

class Workload: public BehaviourNode
{
    Q_OBJECT
public:
    Workload();
    ~Workload();
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // WORKLOAD_H
