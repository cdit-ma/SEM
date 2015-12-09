#include "ineventport.h"

InEventPort::InEventPort():EventPort(true)
{

}

InEventPort::~InEventPort()
{
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}
