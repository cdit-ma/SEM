#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "behaviournode.h"

class OutEventPortImpl : public BehaviourNode
{
    Q_OBJECT
public:
    OutEventPortImpl();
    ~OutEventPortImpl();
public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
};

#endif
