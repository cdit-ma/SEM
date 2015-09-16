#ifndef PROCESS_H
#define PROCESS_H
#include "behaviournode.h"

class Process: public BehaviourNode
{
    Q_OBJECT
public:
    Process();
    ~Process();

public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};


#endif // PROCESS_H
