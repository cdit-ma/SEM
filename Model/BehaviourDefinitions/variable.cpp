#include "variable.h"
#include <QDebug>
Variable::Variable():Node()
{

}

Variable::~Variable()
{
}

QString Variable::toString()
{
    return QString("Variable[%1]: "+this->getName()).arg(this->getID());
}

bool Variable::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Variable::canAdoptChild(Node *child)
{
    return Node::canConnect(child);
}
