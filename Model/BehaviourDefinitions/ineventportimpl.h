#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "behaviournode.h"

class InEventPortImpl : public BehaviourNode
{
    Q_OBJECT
public:
    InEventPortImpl();
    ~InEventPortImpl();

public:
    bool canAdoptChild(Node* child);
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
};


#endif // INEVENTPORT_H
