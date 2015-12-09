#include "ineventportdelegate.h"

InEventPortDelegate::InEventPortDelegate():EventPortDelegate(true)
{
}

InEventPortDelegate::~InEventPortDelegate()
{
}

bool InEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}
