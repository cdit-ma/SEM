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

bool Vector::canConnect(Node* attachableObject)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);

    if(!aggregate){
        return false;
    }

    if(aggregate && edgeCount() > 0){
        #ifdef DEBUG_MODE
            qWarning() << "Can only connect a Vector to one aggregate.";
        #endif
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool Vector::canAdoptChild(Node *child)
{

    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!member && !aggregateInstance){
#ifdef DEBUG_MODE
        qWarning() << "Vector can only adopt Member/Instances";
#endif
        return false;
    }

    if(childrenCount() != 0){
#ifdef DEBUG_MODE
        qWarning() << "Vector can only one Member/Instances";
#endif
        return false;
    }




    return Node::canAdoptChild(child);
}
