#ifndef INEVENTPORT_H
#define INEVENTPORT_H

#include "eventport.h"

class InEventPortInstance;
class InEventPortImpl;

class InEventPort : public EventPort
{
public:
    InEventPort(QString name="");
    ~InEventPort();

    
    bool canAdoptChild(Node *node);
    bool canConnect(Node *node);
};

#endif // INEVENTPORT_H
