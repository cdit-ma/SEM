#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "eventportdelegate.h"

class InEventPortDelegate: public EventPortDelegate
{
    Q_OBJECT
public:
    InEventPortDelegate();
    ~InEventPortDelegate();

    bool canAdoptChild(Node* child);
    bool canConnect_AssemblyEdge(Node *node);
};

#endif // INEVENTPORTDELEGATE_H
