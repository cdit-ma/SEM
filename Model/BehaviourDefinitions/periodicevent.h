#ifndef PERIODICEVENT_H
#define PERIODICEVENT_H
#include "../node.h"

class PeriodicEvent: public Node
{
    Q_OBJECT
public:
    PeriodicEvent();
    ~PeriodicEvent();

    // GraphML interface
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // PERIODICEVENT_H

