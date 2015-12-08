#include "aggregate.h"
#include "member.h"
#include "aggregateinstance.h"
#include "vectorinstance.h"
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

Edge::EDGE_CLASS Aggregate::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool Aggregate::canAdoptChild(Node *child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!member && !aggregateInstance && !vectorInstance){
#ifdef DEBUG_MODE
        qWarning() << "Aggregate can only adopt Member/Instances";
#endif
        return false;
    }
    if(vectorInstance){
        Node* vector = vectorInstance->getDefinition();
        if(vector && vector->hasChildren()){
            qCritical() << "has Defintino with children";
            Node* vectorChild = vector->getChildren(0)[0];
            if(vectorChild && vectorChild->getDefinition()){

               Node* aggregate = vectorChild->getDefinition();
               qCritical() << "Got Aggregate";
               qCritical() << aggregate;
               if(this == aggregate || isAncestorOf(aggregate)){
                   return false;
               }
            }
        }
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
