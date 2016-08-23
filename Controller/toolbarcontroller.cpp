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
    setupEdgeActions();


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(Theme::theme(), SIGNAL(preloadFinished()), this, SLOT(themeChanged()));

    connect(viewController->getSelectionController(), SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged(int)));

    //Connect to the view controller

    connect(viewController, &ViewController::vc_viewItemConstructed, this, &ToolActionController::viewItem_Constructed);
    connect(viewController, &ViewController::vc_viewItemDestructing, this, &ToolActionController::viewItem_Destructed);
}

QList<NodeViewItemAction *> ToolActionController::getDefinitionNodeActions(QString kind)
{
    QList<NodeViewItemAction*> list;

    foreach(ViewItem* item, viewController->getConstructableNodeDefinitions(kind)){
        list.append(getNodeViewItemAction(item->getID()));

    }
    return list;
}

void ToolActionController::viewItem_Constructed(ViewItem *viewItem)
{
    if(viewItem && viewItem->isNode()){
        int ID = viewItem->getID();

        NodeViewItem* node = (NodeViewItem*)viewItem;

        //VIEW_ASPECT aspect = node->getViewAspect();

        if(!actions.contains(ID)){
            NodeViewItemAction* action = new NodeViewItemAction(node);

            if(node->getParentItem() && node->getParentItem()->isNode()){

                int parentID = node->getParentID();
                if(!actions.contains(parentID)){
                    //Construct Parent for menus which need depth
                    NodeViewItemAction* parentAction = new NodeViewItemAction((NodeViewItem*) node->getParentItem());
                    actions[parentID] = parentAction;
                    actionGroup->addAction(parentAction);
                }

                NodeViewItemAction* parentAction = 0;
                if(actions.contains(parentID)){
                    parentAction = actions[parentID];
                }

                action->setParentNodeViewItemAction(parentAction);

                actions[ID] = action;
                actionGroup->addAction(action);
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
        emit viewController->vc_constructNode(ID, kind, position);
    }
}

void ToolActionController::addEdge(int dstID, Edge::EDGE_CLASS edgeKind)
{
    QList<int> IDs = selectionController->getSelectionIDs();
    if(!IDs.isEmpty()){
        qCritical() << "TEST";
        emit viewController->vc_constructEdge(IDs, dstID, edgeKind);
    }
}

void ToolActionController::addConnectedChildNode(int dstID, QString kind, QPointF position)
{
    int ID = selectionController->getFirstSelectedItemID();
    qCritical() << kind;
    if(ID != -1){
        emit viewController->vc_constructConnectedNode(ID, kind,dstID, Edge::EC_UNDEFINED, position);
    }
}

void ToolActionController::setupToolActions()
{
    createRootAction("EC_DEPLOYMENT_CONNECT", "Deploy Selection", "Actions", "Computer");
    createRootAction("EC_DEPLOYMENT_DISCONNECT", "Remove selection deployment", "Actions", "Computer_Cross");

    // setup menu info actions here
    createRootAction("INFO_NO_VALID_DEPLOYMENT_NODES", "There are no valid nodes available.", "Actions", "Info");
    createRootAction("INFO_NO_UNIMPLEMENTED_COMPONENTS", "There are no IDL files containing unimplemented Component entities.", "Actions", "Info");
    createRootAction("INFO_NO_COMPONENTS", "There are no IDL files containing Component entities.", "Actions", "Info");
    createRootAction("INFO_NO_BLACKBOXES", "There are no IDL files containing BlackBox entities.", "Actions", "Info");
    createRootAction("INFO_NO_AGGREGATES", "There are no IDL files containing Aggregate entities.", "Actions", "Info");
    createRootAction("INFO_NO_VECTORS", "There are no IDL files containing initialised Vector entities.", "Actions", "Info");
    createRootAction("INFO_NO_FUCNTIONS", "There are no available functions.", "Actions", "Info");
    createRootAction("INFO_NO_OUTEVENTPORTS", "The selected entity's definition does not contain any OutEventPort entities.", "Actions", "Info");
}

QList<QAction*> ToolActionController::getNodeActionsOfKind(QString kind, bool stealth)
{
    return QList<QAction*>();
}

QAction *ToolActionController::getNodeActionOfKind(QString kind, bool stealth)
{
    return new RootAction("Nodes: " + kind);
}

QList<NodeViewItemAction *> ToolActionController::getEdgeActionsOfKind(Edge::EDGE_CLASS kind)
{
    QList<NodeViewItemAction*> list;

    foreach(ViewItem* item, viewController->getValidEdges(kind)){
        if(item && actions.contains(item->getID())){
            list.append(actions[item->getID()]);
        }
    }
    return list;
}

RootAction *ToolActionController::getEdgeActionOfKind(Edge::EDGE_CLASS kind)
{
    if(edgeKindActions.contains(kind)){
        return edgeKindActions[kind];
    }
    return 0;
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
    foreach(RootAction* action, nodeKindActions.values()){
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
        action->setIconPath("Items", kind);
        nodeKindActions[kind]= action;
        adoptableKindsGroup->addAction(action);
    }
}

void ToolActionController::setupEdgeActions()
{
    foreach(Edge::EDGE_CLASS edgeKind, Edge::getEdgeClasses()){
        QString edgeName = Edge::getKind(edgeKind);
        RootAction* action = new RootAction(edgeName);
        action->setIconPath("Items", edgeName);
        edgeKindActions[edgeKind] = action;
    }
}

NodeViewItemAction *ToolActionController::getNodeViewItemAction(int ID)
{
    if(actions.contains(ID)){
        return actions[ID];
    }
    return 0;
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

