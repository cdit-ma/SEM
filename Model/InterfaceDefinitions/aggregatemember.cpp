#include "aggregatemember.h"
#include "aggregate.h"
#include <QDebug>


AggregateMember::AggregateMember(QString name):Node(Node::NT_DEFINITION)
{
    def = 0;
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
    if (!aggregate || edgeCount() > 0){
        qWarning() << "Can only connect to one Aggregate!";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool AggregateMember::canAdoptChild(Node *child)
{
    return false;
}
