#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "eventportdelegate.h"

class OutEventPortDelegate: public EventPortDelegate
{
    Q_OBJECT
public:
    OutEventPortDelegate();
    ~OutEventPortDelegate();

    bool canAdoptChild(Node* child);
    bool canConnect_AssemblyEdge(Node *node);
};


#endif // OUTEVENTPORTDELEGATE_H
