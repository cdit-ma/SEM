#ifndef PERIODICEVENT_H
#define PERIODICEVENT_H
#include "../node.h"
#include <Qstring>

class PeriodicEvent: public Node
{
    Q_OBJECT
public:
    PeriodicEvent(QString name = "");
    ~PeriodicEvent();

    // GraphML interface
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // PERIODICEVENT_H

