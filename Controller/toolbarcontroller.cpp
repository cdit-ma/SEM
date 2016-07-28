#include "toolbarcontroller.h"
#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include <QDebug>

ToolActionController::ToolActionController(ViewController *viewController):QObject(viewController)
{
    interfaceKinds << "BlackBox" << "Component" << "Aggregate" << "Vector" << "InEventPort" << "OutEventPort";
    kindsWithSubActions << "BlackBoxInstance" << "ComponentInstance" << "ComponentImpl";
    kindsWithSubActions << "AggregateInstance" << "VectorInstance" << "InEventPort";
    kindsWithSubActions << "OutEventPort" << "InEventPortDelegate" << "OutEventPortDelegate";
    kindsWithSubActions << "OutEventPortImpl" << "WorkerProcess";


    toolbar = new QToolBar();
    toolbar->setIconSize(QSize(80,80));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    actionGroup = new ActionGroup();
    adoptableKindsGroup = new ActionGroup();
    this->viewController = viewController;
    setupNodeActions();

    connect(viewController->getSelectionController(), SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged(int)));

    //Connect to the view controller
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), this, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), this, SLOT(viewItem_Destructed(int,ViewItem*)));

}

void ToolActionController::viewItem_Constructed(ViewItem *viewItem)
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
                if(kind == "Process"){
                    ignore = false;
                }
            }else if(aspect == VA_INTERFACES){
               if(kind.endsWith("EventPort")){
                   ignore = false;
               }
            }
            if(!ignore){
                NodeViewItemAction* action = new NodeViewItemAction(item);
                if(!actions.contains(ID)){

                    if(item->getParentItem() && item->getParentItem()->isNode()){
                        int parentID = item->getParentID();
                        if(!actions.contains(parentID)){
                            //Construct Parent for Menus which need depth
                            NodeViewItemAction* parentAction = new NodeViewItemAction((NodeViewItem*) item->getParentItem());
                            action->setParentNodeViewItemAction(parentAction);
                            actions[item->getParentID()] = parentAction;
                            actionGroup->addAction(parentAction);
                            toolbar->addAction(parentAction);
                        }
                    }
                    actions[ID] = action;
                    actionGroup->addAction(action);
                    toolbar->addAction(action);
                }
            }
        }
    }
}

void ToolActionController::viewItem_Destructed(int ID, ViewItem *viewItem)
{
    if(actions.contains(ID)){
        NodeViewItemAction* action = actions[ID];
        actions.remove(ID);
        actionGroup->removeAction(action);
        //toolbar->removeAction(action
        action->deleteLater();
    }
}

void ToolActionController::selectionChanged(int selected)
{
    foreach(QAction* action, adoptableKindsGroup->actions()){
        action->setEnabled(selected != 0);
    }
}

void ToolActionController::addChildNode()
{
    if(sender()){
        qCritical() << sender()->property("kind");
    }
}

QList<QAction *> ToolActionController::getAdoptableKindsActions(bool stealth)
{
    QList<QAction*> actions;
    foreach(RootAction* action, adoptableKindsGroup->getRootActions()){
        actions.append(action->constructSubAction(stealth));
    }
    return actions;
}

QAction *ToolActionController::getAdoptableKindsAction(bool stealth)
{
    return adoptableKindsGroup->getGroupVisibilityAction()->constructSubAction(stealth);
}

QList<NodeViewItemAction *> ToolActionController::getRequiredSubActionsForKind(QString kind)
{
    return actions.values();
}

QStringList ToolActionController::getKindsRequiringSubActions()
{
    return kindsWithSubActions;
}

/**
 * @brief ToolActionController::setupNodeActions
 * Construct a RootAction for each node kind in the ViewController, it hashes them and puts them in adoptableKindsGroup
 */
void ToolActionController::setupNodeActions()
{
    foreach(QString kind, viewController->getNodeKinds()){
        RootAction* action = new RootAction(kind);
        action->setIcon(Theme::theme()->getIcon("Items", kind));
        nodeKindActions[kind]= action;
        adoptableKindsGroup->addAction(action);
    }
}
