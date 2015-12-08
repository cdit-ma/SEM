#ifndef EVENTPORTDELEGATE_H
#define EVENTPORTDELEGATE_H
#include "../node.h"
#include "eventportinstance.h"

class EventPortDelegate : public Node
{
    Q_OBJECT
public:
    EventPortDelegate(bool inEventPortDelegate);
    ~EventPortDelegate();

    bool isInEventPortDelegate();
    bool isOutEventPortDelegate();

    EventPortInstance* getConnectedEventPortInstance();

    bool canAdoptChild(Node*);
    bool canConnect_AssemblyEdge(Node *node);

private:
    bool inEventPortDelegate;
};

#endif // EVENTPORTDELEGATE_H
