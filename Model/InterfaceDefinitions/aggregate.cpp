#include "aggregate.h"

#include "aggregateinstance.h"
#include "member.h"
#include "vectorinstance.h"
#include "eventport.h"

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
    int index = attachedEventPorts.indexOf(node);
    if(index != -1){
        attachedEventPorts.remove(index);
    }
}

QVector<EventPort *> Aggregate::getEventPorts()
{
    return attachedEventPorts;
}


bool Aggregate::canAdoptChild(Node *child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!(aggregateInstance || member || vectorInstance)){
        return false;
    }

    //TODO may not need.
    if(vectorInstance){
        Node* vector = vectorInstance->getDefinition();
        if(vector && vector->hasChildren()){
            //Check first child of vector.
            Node* vectorChild = vector->getChildren(0)[0];

            //If first child has a definition.
            if(vectorChild && vectorChild->getDefinition()){
               Node* aggregate = vectorChild->getDefinition();
               if(this == aggregate || isAncestorOf(aggregate)){
                   return false;
               }
            }
        }
    }

    return Node::canAdoptChild(child);
}
