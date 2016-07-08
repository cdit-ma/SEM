#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"

#include <QDebug>
ViewController::ViewController(){
}

void ViewController::setDefaultIcon(ViewItem *viewItem)
{
    if(viewItem->isNode()){
        QString nodeKind = viewItem->getData("kind").toString();
        viewItem->setDefaultIcon("Items", nodeKind);
        qCritical() << viewItem->getIcon();
    }
}

void ViewController::entityConstructed(EntityAdapter *entity)
{
    ViewItem* viewItem = 0;

    if(entity->isNodeAdapter()){
        viewItem = new NodeViewItem((NodeAdapter*)entity);


    }else if(entity->isEdgeAdapter()){
        viewItem = new EdgeViewItem((EdgeAdapter*)entity);
    }

    if(viewItem){
        int ID = viewItem->getID();
        viewItems[ID] = viewItem;
        setDefaultIcon(viewItem);

        //Tell Views
        emit viewItemConstructed(viewItem);
    }

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
                emit viewItemDestructing(ID);

                qCritical() << "VC: Destructing: " << viewItem;
                viewItem->destruct();
            }
        }
    }
}

