#include "aggregatemember.h"
#include "aggregate.h"
#include "../BehaviourDefinitions/memberinstance.h"
#include <QDebug>


AggregateMember::AggregateMember(QString name):Node(Node::NT_DEFINSTANCE)
{
}

AggregateMember::~AggregateMember()
{

}

QString AggregateMember::toString()
{
    return QString("AggregateMember[%1]: "+this->getName()).arg(this->getID());
}

bool AggregateMember::canConnect(Node* attachableObject)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(attachableObject);

    if(!aggregate && !aggregateMember){
        qWarning() << "AggregateMember can only connect to an Aggregate or AggregateMember.";
        return false;
    }
    /*if (edgeCount() > 0){
        return false;
    }*/

    return Node::canConnect(attachableObject);
}

bool AggregateMember::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(child);

    if(!memberInstance && !aggregateMember){
        qWarning() << "AggregateMember can only adopt MemberInstance or AggregateMember";
        return false;
    }

    return Node::canAdoptChild(child);
}
