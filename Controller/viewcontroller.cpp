#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"

#include <QDebug>
ViewController::ViewController(){
    selectionHandler = new SelectionHandler();
}

SelectionHandler *ViewController::getSelectionHandler(QObject *object)
{
    return selectionHandler;
}

void ViewController::setDefaultIcon(ViewItem *viewItem)
{
    if(viewItem->isNode()){

        QString nodeKind = viewItem->getData("kind").toString();
        QString imageName = nodeKind;
        if(nodeKind == "HardwareNode"){
            bool localhost = viewItem->hasData("localhost") && viewItem->getData("localhost").toBool();

            if(localhost){
                imageName = "Localhost";
            }else{
                QString os = viewItem->getData("os").toString();
                QString arch = viewItem->getData("architecture").toString();
                imageName = os + "_" + arch;
                imageName = imageName.remove(" ");
            }
        }
        viewItem->setDefaultIcon("Items", imageName);
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

            //Unset.
            selectionHandler->itemDeleted(viewItem);

            if(viewItem){
                emit viewItemDestructing(ID, viewItem);
                viewItem->destruct();
            }
        }
    }
}

