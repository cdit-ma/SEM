#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"
#include "eventport.h"

class Aggregate : public Node
{
    Q_OBJECT
public:
    Aggregate();
    ~Aggregate();


    void addEventPort(EventPort* node);
    void removeEventPort(EventPort* node);
    QVector<EventPort*> getEventPorts();

    bool canAdoptChild(Node* child);

private:
    QVector<EventPort*> attachedEventPorts;
};


#endif // AGGREGATE_H
