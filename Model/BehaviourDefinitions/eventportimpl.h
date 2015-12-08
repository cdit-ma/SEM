#ifndef EVENTPORTIMPL_H
#define EVENTPORTIMPL_H
#include "behaviournode.h"

class EventPortImpl : public BehaviourNode
{
    Q_OBJECT
public:
    EventPortImpl(bool isInEventPort);
    ~EventPortImpl();

    bool isInEventPort();
    bool isOutEventPort();

    bool canAdoptChild(Node* child);

    bool canConnect_DefinitionEdge(Node *definition);

private:
    bool inEventPort;
};

#endif // EVENTPORTIMPL_H
