#include "aggregatemember.h"
#include "aggregate.h"


AggregateMember::AggregateMember(QString name):Node()
{
    def = 0;
}

AggregateMember::~AggregateMember()
{

}

void AggregateMember::setDefinition(Aggregate *def)
{
    this->def = def;

}

Aggregate *AggregateMember::getDefinition()
{
    return def;
}

QString AggregateMember::toString()
{
    return QString("AggregateMember[%1]: "+this->getName()).arg(this->getID());
}

bool AggregateMember::canConnect(Node* attachableObject)
{
    if(this->getEdges(0).size() != 1){
        return false;
    }else{
        Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);
        if (!aggregate){
            return false;
        }
    }

    return Node::canConnect(attachableObject);
}

bool AggregateMember::canAdoptChild(Node *child)
{
    return false;
}
