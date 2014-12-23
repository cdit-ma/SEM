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
    //qCritical() << "Removing Aggregate";
    int index = attachedEventPorts.indexOf(node);
    if(index != -1){
        attachedEventPorts.remove(index);
    }
}

QVector<EventPort *> Aggregate::getEventPorts()
{
    return attachedEventPorts;
}

QString Aggregate::toString()
{
    return QString("Aggregate[%1]: "+this->getName()).arg(this->getID());
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
        qWarning() << "Aggregate can only adopt Member/Instances";
        return false;
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
