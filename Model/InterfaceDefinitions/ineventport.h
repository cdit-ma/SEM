#ifndef INEVENTPORT_H
#define INEVENTPORT_H

#include "eventport.h"

class InEventPortInstance;
class InEventPortImpl;

class InEventPort : public EventPort
{
public:
    InEventPort();
    ~InEventPort();

    
    bool canAdoptChild(Node *node);
    Edge::EDGE_CLASS canConnect(Node *node);
};

#endif // INEVENTPORT_H
