#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "eventport.h"


class OutEventPort : public EventPort
{
public:
    OutEventPort(QString name="");
    ~OutEventPort();

    

    // Node interface
    bool canAdoptChild(Node *node);
    bool canConnect(Node *node);
};

#endif // OUTEVENTPORT_H
