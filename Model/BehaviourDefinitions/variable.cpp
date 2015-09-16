#include "variable.h"
#include <QDebug>
Variable::Variable():Node()
{

}

Variable::~Variable()
{
}

bool Variable::canConnect(Node*)
{
    return false;
}

bool Variable::canAdoptChild(Node*)
{
    return false;
}
