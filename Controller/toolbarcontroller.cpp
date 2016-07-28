#include "toolbarcontroller.h"
#include "viewcontroller.h"
#include "../View/nodeviewitem.h"

ToolbarController::ToolbarController(ViewController *viewController):QObject(viewController)
{
    toolbar = new QToolBar();
    actionGroup = new ActionGroup();

    //Connect to the view controller
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), this, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), this, SLOT(viewItem_Destructed(int,ViewItem*)));

}

void ToolbarController::viewItem_Constructed(ViewItem *viewItem)
{
    //We only care about NOdes.
    if(viewItem && viewItem->isNode()){
        int ID = viewItem->getID();

        NodeViewItem* item = (NodeViewItem*)viewItem;

        VIEW_ASPECT aspect = item->getViewAspect();

        //We only care about Interfaces and workers.
        if(aspect == VA_INTERFACES || aspect == VA_WORKERS){
            QString kind = viewItem->getData("kind").toString();
            bool ignore = true;
            if(aspect == VA_WORKERS){
                if(kind == "Workload" || kind == "Process"){
                    ignore = false;
                }
            }else if(aspect == VA_INTERFACES){

            }
            if(!ignore){
                NodeViewItemAction* action = new NodeViewItemAction(item);
                if(!actions.contains(ID)){
                    actions[ID] = action;
                    actionGroup->addAction(action);
                    toolbar->addAction(action);
                }
            }
        }
    }
}

void ToolbarController::viewItem_Destructed(int ID, ViewItem *viewItem)
{
    if(actions.contains(ID)){
        NodeViewItemAction* action = actions[ID];
        actions.remove(ID);
        actionGroup->removeAction(action);
        toolbar->removeAction(action);
        action->deleteLater();
    }
}
