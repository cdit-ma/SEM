#include "edgeviewitem.h"


EdgeViewItem::EdgeViewItem(EdgeAdapter *entity):ViewItem(entity)
{
    this->entity = entity;
}

EdgeViewItem::~EdgeViewItem()
{

}

int EdgeViewItem::getSourceID()
{
    int ID = -1;
    if(entity){
        ID = entity->getSourceID();
    }
    return ID;
}

int EdgeViewItem::getDestinationID()
{
    int ID = -1;
    if(entity){
        ID = entity->getDestinationID();
    }
    return ID;
}
