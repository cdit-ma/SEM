#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "eventport.h"


class OutEventPort : public EventPort
{
    Q_OBJECT
public:
    OutEventPort();
    ~OutEventPort();

    bool canAdoptChild(Node *node);
};

#endif // OUTEVENTPORT_H
