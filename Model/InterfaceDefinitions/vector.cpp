#include <QDebug>
#include "aggregate.h"
#include "vector.h"
#include "member.h"
#include "aggregateinstance.h"
#include "vectorinstance.h"

Vector::Vector(): Node(Node::NT_DEFINITION)
{
}

Vector::~Vector()
{
}

bool Vector::canAdoptChild(Node *child)
{

    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!(aggregateInstance || member)){
        return false;
    }
    if(hasChildren()){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool Vector::canConnect_AggregateEdge(Node *node)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(node);

    if(!aggregate){
        return false;
    }

    if(hasChildren()){
        return false;
    }
    return Node::canConnect_AggregateEdge(aggregate);

}
