#include "outeventportdelegate.h"

OutEventPortDelegate::OutEventPortDelegate():EventPortDelegate(false)
{
}

OutEventPortDelegate::~OutEventPortDelegate()
{
}

bool OutEventPortDelegate::canAdoptChild(Node *)
{
    return false;
}
