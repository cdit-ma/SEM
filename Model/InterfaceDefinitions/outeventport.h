#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "eventport.h"


class OutEventPort : public EventPort
{
public:
    OutEventPort();
    ~OutEventPort();

    

    // Node interface
    bool canAdoptChild(Node *node);
    bool canConnect(Node *node);
};

#endif // OUTEVENTPORT_H
