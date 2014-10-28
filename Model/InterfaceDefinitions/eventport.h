#ifndef EVENTPORT_H
#define EVENTPORT_H
#include "../node.h"

class EventPort : public Node
{
public:
    EventPort();
    ~EventPort();

    // GraphML interface
    QString toString();

    void setAggregate(Node *aggregate);
    Node* getAggregate();
    void unsetAggregate();

    bool canAdoptChild(Node *node);
    bool canConnect(Node *node);

private:
    Node* aggregate;
};

#endif // EVENTPORT_H
