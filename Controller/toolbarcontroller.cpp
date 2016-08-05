#include "toolbarcontroller.h"
#include "viewcontroller.h"
#include "../View/nodeviewitem.h"
#include "../Widgets/New/selectioncontroller.h"
#include <QDebug>

ToolActionController::ToolActionController(ViewController *viewController):QObject(viewController)
{
    interfaceKinds << "BlackBox" << "Component" << "Aggregate" << "Vector" << "InEventPort" << "OutEventPort";
    kindsWithSubActions << "BlackBoxInstance" << "ComponentInstance" << "ComponentImpl";
    kindsWithSubActions << "AggregateInstance" << "VectorInstance" << "InEventPort";
    kindsWithSubActions << "OutEventPort" << "InEventPortDelegate" << "OutEventPortDelegate";
    kindsWithSubActions << "OutEventPortImpl" << "WorkerProcess";


    //toolbar = new QToolBar();
    //toolbar->setIconSize(QSize(80,80));
    //toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    actionGroup = new ActionGroup();
    adoptableKindsGroup = new ActionGroup();
    this->viewController = viewController;
    this->selectionController = viewController->getSelectionController();
    setupNodeActions();
    setupToolActions();


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
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
/*
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
            if(!ignore){*/
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
                            //toolbar->addAction(parentAction);
                        }
                    }
                    actions[ID] = action;
                    actionGroup->addAction(action);
                    //toolbar->addAction(action);
                }
            //}
        //}
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
    QStringList validActions;

    if(selected == 1){
        validActions = viewController->getAdoptableNodeKinds();
    }

    foreach(QAction* action, adoptableKindsGroup->actions()){
        action->setEnabled(validActions.contains(action->text()));
    }
}

void ToolActionController::addChildNode(QString kind, QPointF position)
{
    ViewItem* item = selectionController->getFirstSelectedItem();

    if(item){
        int ID = item->getID();
        emit viewController->constructChildNode(ID, kind, position);
    }
}

void ToolActionController::setupToolActions()
{
    createRootAction("EC_DEPLOYMENT_CONNECT", "Deploy Selection", "Actions", "Computer");
    createRootAction("EC_DEPLOYMENT_DISCONNECT", "Remove selection deployment", "Actions", "Computer_Cross");

    //createRootAction("APPLY_REPLICATE_COUNT", "Enter Replicate Count", "Actions", "Tick");

    // setup menu info actions here
    createRootAction("NO_EC_DEPLOYMENT_CONNECT", "NONE AVAILABLE", "Actions", "Info");
}

QList<QAction*> ToolActionController::getNodeActionsOfKind(QString kind, bool stealth)
{
    return QList<QAction*>();
}

QAction *ToolActionController::getNodeActionOfKind(QString kind, bool stealth)
{
    return new RootAction("Nodes: " + kind);
}

QList<QAction*> ToolActionController::getEdgeActionsOfKind(Edge::EDGE_CLASS kind, bool stealth)
{
    QList<QAction*> list;

    foreach(int ID, viewController->getValidEdges(kind)){
        list.append(actions[ID]->constructSubAction(stealth));
    }
    return list;
}

QAction* ToolActionController::getEdgeActionOfKind(Edge::EDGE_CLASS kind, bool stealth)
{
    return new RootAction("Edges: " + kind);
}

QList<QAction*> ToolActionController::getAdoptableKindsActions(bool stealth)
{
    QList<QAction*> actions;
    foreach(RootAction* action, adoptableKindsGroup->getRootActions()){
        actions.append(action->constructSubAction(stealth));
    }
    return actions;
}

QAction* ToolActionController::getAdoptableKindsAction(bool stealth)
{
    return adoptableKindsGroup->getGroupVisibilityAction()->constructSubAction(stealth);
}

QList<QAction*> ToolActionController::getConnectedNodesActions(bool stealth)
{
    return QList<QAction*>();
}

QAction* ToolActionController::getConnectedNodesAction(bool stealth)
{
    return new RootAction("Connections");
}

QList<QAction *> ToolActionController::getHardwareActions(bool stealth)
{
    return QList<QAction*>();
}

QAction *ToolActionController::getHardwareAction(bool stealth)
{
    return new RootAction("Hardware");
}

QList<QAction *> ToolActionController::getInstancesActions(bool stealth)
{
    return QList<QAction*>();
}

QAction *ToolActionController::getInstancesAction(bool stealth)
{
    return new RootAction("Instances");
}

QAction *ToolActionController::getToolAction(QString hashKey, bool stealth)
{
    if(toolActions.contains(hashKey)){
        return toolActions[hashKey]->constructSubAction(stealth);
    }
    return 0;
}

QList<NodeViewItemAction*> ToolActionController::getRequiredSubActionsForKind(QString kind)
{
    return actions.values();
}

void ToolActionController::themeChanged()
{
    Theme* theme = Theme::theme();
    foreach(RootAction* action, toolActions.values()){
        viewController->getActionController()->updateIcon(action, theme);
    }
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

RootAction *ToolActionController::createRootAction(QString hashKey, QString actionName, QString iconPath, QString aliasPath)
{
    if(!toolActions.contains(hashKey)){
        RootAction* action = new RootAction(actionName, this);
        action->setIconPath(iconPath, aliasPath);
        toolActions[hashKey] = action;
    }
    if(toolActions.contains(hashKey)){
        return toolActions[hashKey];
    }
    return 0;
}

