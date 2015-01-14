#include "variable.h"
#include <QDebug>
Variable::Variable():Node()
{

}

Variable::~Variable()
{
}

bool Variable::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool Variable::canAdoptChild(Node *child)
{
    return Node::canConnect(child);
}
