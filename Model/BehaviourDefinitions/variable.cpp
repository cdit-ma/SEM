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
    return false;
}

bool Variable::canAdoptChild(Node *child)
{
    return false;
}
