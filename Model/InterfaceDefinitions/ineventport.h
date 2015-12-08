#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "eventport.h"

class InEventPort : public EventPort
{
    Q_OBJECT
public:
    InEventPort();
    ~InEventPort();
    
    bool canAdoptChild(Node *node);
};

#endif // INEVENTPORT_H
