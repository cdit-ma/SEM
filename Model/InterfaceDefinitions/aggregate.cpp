#include "aggregate.h"
#include "member.h"
#include "aggregatemember.h"
Aggregate::Aggregate(QString name): Node(name)
{

}

Aggregate::~Aggregate()
{

}

void Aggregate::addMember(AggregateMember *member)
{
    if(!members.contains(member)){
        members.append(member);
        member->setDefinition(this);
    }
}

void Aggregate::removeMember(AggregateMember *member)
{
    int index = members.indexOf(member);
    if(index >= 0){
        member->setDefinition(0);
        members.removeAt(index);
    }
}

QVector<AggregateMember *> Aggregate::getMembers()
{
    return members;
}

QString Aggregate::toString()
{
    return QString("Aggregate[%1]: "+this->getName()).arg(this->getID());
}

bool Aggregate::canConnect(Node* attachableObject)
{
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(attachableObject);
    if (aggregateMember){
        return true;
    }

    return false;
}

bool Aggregate::canAdoptChild(Node *child)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(child);
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(child);
    Member* member = dynamic_cast<Member*>(child);

    if(!aggregate && !member && !aggregateMember){
        return false;
    }

    //Check for loops
    if(aggregateMember){
        Aggregate* definition = aggregateMember->getDefinition();
        if(definition && isAncestorOf(definition)){
            return false;
        }
    }

    return true;
}
