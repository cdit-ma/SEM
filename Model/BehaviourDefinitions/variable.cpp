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
    Q_UNUSED(attachableObject);
    return false;
}

bool Variable::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
