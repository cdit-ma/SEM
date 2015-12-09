#include "outeventport.h"

OutEventPort::OutEventPort():EventPort(false)
{
}

OutEventPort::~OutEventPort()
{

}

bool OutEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

