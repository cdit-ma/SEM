#include "aggregate.h"
#include "member.h"
#include "aggregatemember.h"
#include "eventport.h"
#include <QDebug>
Aggregate::Aggregate(): Node()
{

}

Aggregate::~Aggregate()
{

}

QString Aggregate::toString()
{
    return QString("Aggregate[%1]: "+this->getName()).arg(this->getID());
}

bool Aggregate::canConnect(Node* attachableObject)
{
    EventPort* eventport = dynamic_cast<EventPort*>(attachableObject);
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(attachableObject);

    if (!aggregateMember && !eventport){
        qWarning() << "Aggregate can only connect to an Aggregate member or an Eventport";
        return false;
    }

    if(eventport && eventport->getAggregate()){
        qWarning() << "Aggregate can only connect to an unn-attached Eventport";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool Aggregate::canAdoptChild(Node *child)
{
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!member && !aggregateMember){
        qWarning() << "Aggregate can only adopt Member/Aggregates";
        return false;
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
