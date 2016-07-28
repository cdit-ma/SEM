#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"
#include <QDebug>
ViewController::ViewController(){
    modelItem = 0;
    _modelReady = false;
    selectionController = new SelectionController(this);
    actionController = new ActionController(this);
    actionController->connectSelectionController(selectionController);
    toolbarController = new ToolbarController(this);
    toolbar = new ToolbarWidgetNew(actionController);
    connect(this, SIGNAL(modelReady(bool)), actionController, SLOT(modelReady(bool)));
    emit modelReady(false);
}

SelectionController *ViewController::getSelectionController()
{
    return selectionController;
}

ActionController *ViewController::getActionController()
{
    return actionController;
}

ToolbarController *ViewController::getToolbarController()
{
    return toolbarController;
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
        }else if(nodeKind == "Process"){


        }
        viewItem->setDefaultIcon("Items", imageName);
    }
}

ViewItem *ViewController::getModel()
{
    return modelItem;
}

bool ViewController::isModelReady()
{
    return _modelReady;
}

void ViewController::showToolbar(QPointF pos)
{
    toolbar->move(pos.toPoint());
    toolbar->show();
    //toolbar->setVisible(true);
}

void ViewController::setModelReady(bool okay)
{
    if(okay != _modelReady){
        _modelReady = okay;
        emit modelReady(okay);
    }
}

void ViewController::entityConstructed(EntityAdapter *entity)
{
    ViewItem* viewItem = 0;
    bool isModel = false;
    if(entity->isNodeAdapter()){
        NodeAdapter* nodeAdapter = (NodeAdapter*)entity;
        int parentID = nodeAdapter->getParentNodeID();

        viewItem = new NodeViewItem(nodeAdapter);

        //Attach the node to it's parent
        if(viewItems.contains(parentID)){
            ViewItem* parent = viewItems[parentID];
            parent->addChild(viewItem);
        }

        if(entity->getDataValue("kind") == "Model"){
            isModel = true;
        }
    }else if(entity->isEdgeAdapter()){
        viewItem = new EdgeViewItem((EdgeAdapter*)entity);
    }

    if(viewItem){
        int ID = viewItem->getID();
        viewItems[ID] = viewItem;
        setDefaultIcon(viewItem);

        if(isModel){
            modelItem = viewItem;
        }

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


            //Unset modelItem
            if(viewItem == modelItem){
                modelItem = 0;
            }

            ViewItem* parentItem = viewItem->getParentItem();
            if(parentItem){
                parentItem->removeChild(viewItem);
            }

            //Remove the item from the Hash
            viewItems.remove(ID);

            if(viewItem){
                emit viewItemDestructing(ID, viewItem);
                viewItem->destruct();
            }
        }
    }
}

