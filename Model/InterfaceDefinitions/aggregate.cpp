#include "aggregate.h"
#include "member.h"
#include "aggregateinstance.h"
#include "eventport.h"
#include <QDebug>
Aggregate::Aggregate(): Node(Node::NT_DEFINITION)
{

}

Aggregate::~Aggregate()
{

}

void Aggregate::addEventPort(EventPort *node)
{
    if(!attachedEventPorts.contains(node)){
        attachedEventPorts.append(node);
        node->setAggregate(this);
    }
}

void Aggregate::removeEventPort(EventPort *node)
{
    //qWarning() << "Removing Aggregate";
    int index = attachedEventPorts.indexOf(node);
    if(index != -1){
        attachedEventPorts.remove(index);
    }
}

QVector<EventPort *> Aggregate::getEventPorts()
{
    return attachedEventPorts;
}

bool Aggregate::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool Aggregate::canAdoptChild(Node *child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!member && !aggregateInstance){
#ifdef DEBUG_MODE
        qWarning() << "Aggregate can only adopt Member/Instances";
#endif
        return false;
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
