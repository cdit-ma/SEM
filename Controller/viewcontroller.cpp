#include "viewcontroller.h"
#include <QDebug>
ViewController::ViewController(){
}

void ViewController::entityConstructed(EntityAdapter *entity)
{
    ViewItem* viewItem = new ViewItem(entity);
    int ID = viewItem->getID();
    viewItems[ID] = viewItem;

    //Tell Views
    emit viewItemConstructed(viewItem);
}

void ViewController::entityDestructed(EntityAdapter *entity)
{
    if(entity){
        int ID = entity->getID();
        if(viewItems.contains(ID)){
            ViewItem* viewItem = viewItems[ID];

            //Remove the item from the Hash
            viewItems.remove(ID);

            if(viewItem){
                qCritical() << "VC: Destructing: " << viewItem;
                viewItem->destruct();
            }
        }
    }
}

