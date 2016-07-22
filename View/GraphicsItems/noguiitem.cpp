#include "noguiitem.h"

NoGUIItem::NoGUIItem(EntityAdapter *eA):QObject()
{
    ID = -1;
    entityAdapter = eA;
    if(eA){
        ID = eA->getID();
    }
    entityAdapter->registerObject(this);
}

NoGUIItem::~NoGUIItem()
{
    entityAdapter->unregisterObject(this);
}

int NoGUIItem::getID()
{
    return ID;
}

EntityAdapter *NoGUIItem::getEntityAdapter()
{
    return entityAdapter;
}

