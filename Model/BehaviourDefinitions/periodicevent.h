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

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // PERIODICEVENT_H

