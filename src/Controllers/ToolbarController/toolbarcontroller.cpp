#include "toolbarcontroller.h"
#include "../ViewController/viewcontroller.h"
#include "../ViewController/nodeviewitem.h"
#include "../SelectionController/selectioncontroller.h"
#include "../../Utils/rootaction.h"

#include <QDebug>

ToolbarController::ToolbarController(ViewController *viewController):QObject(viewController)
{
    interfaceKinds << "BlackBox" << "Component" << "Aggregate" << "Vector" << "InEventPort" << "OutEventPort";

    kindsWithSubActions << "BlackBoxInstance" << "ComponentInstance" << "ComponentImpl";
    kindsWithSubActions << "AggregateInstance" << "VectorInstance" << "InEventPort";
    kindsWithSubActions << "OutEventPort" << "InEventPortDelegate" << "OutEventPortDelegate";
    kindsWithSubActions << "OutEventPortImpl" << "WorkerProcess";

    infoActionKeyHash["BlackBoxInstance"] = "INFO_NO_BLACKBOXES";
    infoActionKeyHash["ComponentInstance"] = "INFO_NO_COMPONENTS";
    infoActionKeyHash["ComponentImpl"] = "INFO_NO_UNIMPLEMENTED_COMPONENTS";
    infoActionKeyHash["AggregateInstance"] = "INFO_NO_AGGREGATES";
    infoActionKeyHash["VectorInstance"] = "INFO_NO_VECTORS";
    infoActionKeyHash["InEventPort"] = "INFO_NO_AGGREGATES";
    infoActionKeyHash["OutEventPort"] = "INFO_NO_AGGREGATES";
    infoActionKeyHash["InEventPortDelegate"] = "INFO_NO_AGGREGATES";
    infoActionKeyHash["OutEventPortDelegate"] = "INFO_NO_AGGREGATES";
    infoActionKeyHash["OutEventPortImpl"] = "INFO_NO_OUTEVENTPORTS";
    infoActionKeyHash["WorkerProcess"] = "INFO_NO_FUNCTIONS";

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

    connect(selectionController, &SelectionController::selectionChanged, this, &ToolbarController::selectionChanged);

    //Connect to the view controller
    connect(viewController, &ViewController::vc_actionFinished, this, &ToolbarController::actionFinished);
    connect(viewController, &ViewController::vc_viewItemConstructed, this, &ToolbarController::viewItem_Constructed);
    connect(viewController, &ViewController::vc_viewItemDestructing, this, &ToolbarController::viewItem_Destructed);
}

QList<NodeViewItemAction *> ToolbarController::getDefinitionNodeActions(QString kind)
{
    QList<NodeViewItemAction*> list;

    foreach(ViewItem* item, viewController->getConstructableNodeDefinitions(kind)){
        NodeViewItemAction* action = getNodeViewItemAction(item->getID());
        if(action){
            list.append(action);
        }
    }
    return list;
}

QList<NodeViewItemAction *> ToolbarController::getWorkerFunctions()
{
    QList<NodeViewItemAction*> list;

    foreach(ViewItem* item, viewController->getWorkerFunctions()){
        NodeViewItemAction* action = getNodeViewItemAction(item->getID());
        if(action){
            list.append(action);
        }
    }
    return list;
}

NodeViewItemAction *ToolbarController::getNodeAction(int ID)
{
    return actions.value(ID, 0);
}

void ToolbarController::viewItem_Constructed(ViewItem *viewItem)
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

                if(node->getNodeKind() == Node::NK_HARDWARE_NODE || node->getNodeKind() == Node::NK_HARDWARE_CLUSTER){
                    hardwareIDs.append(ID);
                    emit hardwareCreated(ID);
                }
            }
        }
    }
}

void ToolbarController::viewItem_Destructed(int ID, ViewItem *)
{
    if(actions.contains(ID)){
        if(hardwareIDs.contains(ID)){
            emit hardwareDestructed(ID);
        }

        NodeViewItemAction* action = actions[ID];
        actions.remove(ID);
        actionGroup->removeAction(action);
        action->deleteLater();
    }
}

void ToolbarController::selectionChanged(int selected)
{
    QStringList validActions;

    if(selected == 1){
        validActions = viewController->getAdoptableNodeKinds();
    }

    foreach(QAction* action, adoptableKindsGroup->actions()){
        action->setEnabled(validActions.contains(action->text()));
    }

    QList<Edge::EDGE_KIND> validEdges = viewController->getValidEdgeKindsForSelection();
    QList<Edge::EDGE_KIND> existingEdges = viewController->getExistingEdgeKindsForSelection();

    foreach(Edge::EDGE_KIND edgeKind, connectEdgeKindActions.keys()){
        RootAction* action = connectEdgeKindActions[edgeKind];
        action->setEnabled(validEdges.contains(edgeKind));
    }
    foreach(Edge::EDGE_KIND edgeKind, disconnectEdgeKindActions.keys()){
        RootAction* action = disconnectEdgeKindActions[edgeKind];
        action->setEnabled(existingEdges.contains(edgeKind));
    }
}

void ToolbarController::actionFinished()
{
    if(selectionController){
        selectionChanged(selectionController->getSelectionCount());
    }
}

void ToolbarController::addChildNode(QString kind, QPointF position)
{
    ViewItem* item = selectionController->getActiveSelectedItem();

    if(item){
        int ID = item->getID();
        emit viewController->vc_constructNode(ID, kind, position);
    }
}

void ToolbarController::addEdge(int dstID, Edge::EDGE_KIND edgeKind)
{
    QList<int> IDs = selectionController->getSelectionIDs();
    if(!IDs.isEmpty()){
        emit viewController->vc_constructEdge(IDs, dstID, edgeKind);
    }
}

void ToolbarController::removeEdge(int dstID, Edge::EDGE_KIND edgeKind)
{
    QList<int> IDs = selectionController->getSelectionIDs();
    if(!IDs.isEmpty()){
        emit viewController->vc_destructEdges(IDs, dstID, edgeKind);
    }
}

void ToolbarController::addConnectedChildNode(int dstID, QString kind, QPointF position)
{
    int ID = selectionController->getFirstSelectedItemID();
    if(ID != -1){
        emit viewController->vc_constructConnectedNode(ID, kind,dstID, Edge::EC_UNDEFINED, position);
    }
}

void ToolbarController::addWorkerProcess(int processID, QPointF position)
{
    int ID = selectionController->getFirstSelectedItemID();
    if(ID != -1){
        emit viewController->vc_constructWorkerProcess(ID, processID, position);
    }
}

void ToolbarController::actionHoverEnter(int ID)
{
    emit viewController->vc_highlightItem(ID, true);
}

void ToolbarController::actionHoverLeave(int ID)
{
    emit viewController->vc_highlightItem(ID, false);
}

void ToolbarController::setupToolActions()
{
    createRootAction("EC_DEPLOYMENT_CONNECT", "Deploy Selection", "Actions", "Computer");
    createRootAction("EC_DEPLOYMENT_DISCONNECT", "Remove selection deployment", "Actions", "Computer_Cross");

    // setup menu info actions here
    createRootAction("INFO_NO_VALID_DEPLOYMENT_NODES", "There are no available hardware nodes", "Actions", "Info");
    createRootAction("INFO_NO_UNIMPLEMENTED_COMPONENTS", "There are no IDL files containing unimplemented Component entities", "Actions", "Info");
    createRootAction("INFO_NO_COMPONENTS", "There are no IDL files containing Component entities", "Actions", "Info");
    createRootAction("INFO_NO_BLACKBOXES", "There are no IDL files containing BlackBox entities", "Actions", "Info");
    createRootAction("INFO_NO_AGGREGATES", "There are no IDL files containing Aggregate entities", "Actions", "Info");
    createRootAction("INFO_NO_VECTORS", "There are no IDL files containing initialised Vector entities", "Actions", "Info");
    createRootAction("INFO_NO_FUNCTIONS", "There are no available functions", "Actions", "Info");
    createRootAction("INFO_NO_OUTEVENTPORTS", "The selected entity's definition does not contain any OutEventPort entities", "Actions", "Info");
    createRootAction("INFO_NO_VALID_EDGE", "There are no entities with the required kind to connect to", "Actions", "Info");
    createRootAction("INFO_NO_EDGE_TO_DISCONNECT", "There are no edges to disconnect", "Actions", "Info");
}

QList<NodeViewItemAction *> ToolbarController::getEdgeActionsOfKind(Edge::EDGE_KIND kind)
{
    QList<NodeViewItemAction*> list;

    foreach(ViewItem* item, viewController->getValidEdges(kind)){
        if(item && actions.contains(item->getID())){
            list.append(actions[item->getID()]);
        }
    }
    return list;
}

QList<NodeViewItemAction *> ToolbarController::getExistingEdgeActionsOfKind(Edge::EDGE_KIND kind)
{
    QList<NodeViewItemAction*> list;
    foreach(ViewItem* item, viewController->getExistingEdgeEndPointsForSelection(kind)){
        if(item && actions.contains(item->getID())){
            list.append(actions[item->getID()]);
        }
    }
    return list;
}

RootAction *ToolbarController::getConnectEdgeActionOfKind(Edge::EDGE_KIND kind)
{
    if(connectEdgeKindActions.contains(kind)){
        return connectEdgeKindActions[kind];
    }
    return 0;
}

RootAction *ToolbarController::getDisconnectEdgeActionOfKind(Edge::EDGE_KIND kind)
{
    if(disconnectEdgeKindActions.contains(kind)){
        return disconnectEdgeKindActions[kind];
    }
    return 0;
}

QList<QAction*> ToolbarController::getAdoptableKindsActions(bool stealth)
{
    QList<QAction*> actions;
    foreach(RootAction* action, adoptableKindsGroup->getRootActions()){
        QAction* subAction = action->constructSubAction(stealth);
        actions.append(subAction);
    }
    return actions;
}

QAction* ToolbarController::getAdoptableKindsAction(bool stealth)
{
    return adoptableKindsGroup->getGroupVisibilityAction()->constructSubAction(stealth);
}



QAction *ToolbarController::getToolAction(QString hashKey, bool stealth)
{
    if(toolActions.contains(hashKey)){
        return toolActions[hashKey]->constructSubAction(stealth);
    }
    return 0;
}


/**
 * @brief ToolActionController::getInfoActionKeyForAdoptableKind
 * @param kind
 * @return
 */
QString ToolbarController::getInfoActionKeyForAdoptableKind(QString kind)
{
    if (infoActionKeyHash.contains(kind)) {
        return infoActionKeyHash.value(kind);
    }
    return "";
}


void ToolbarController::themeChanged()
{
    Theme* theme = Theme::theme();
    foreach(RootAction* action, toolActions.values()){
        viewController->getActionController()->updateIcon(action, theme);
    }
    foreach(RootAction* action, nodeKindActions.values()){
        viewController->getActionController()->updateIcon(action, theme);
    }
    foreach (RootAction* action, connectEdgeKindActions.values()) {
        viewController->getActionController()->updateIcon(action, theme);
    }
    foreach (RootAction* action, disconnectEdgeKindActions.values()) {
        viewController->getActionController()->updateIcon(action, theme);
    }

}

QStringList ToolbarController::getKindsRequiringSubActions()
{
    return kindsWithSubActions;
}

/**
 * @brief ToolActionController::setupNodeActions
 * Construct a RootAction for each node kind in the ViewController, it hashes them and puts them in adoptableKindsGroup
 */
void ToolbarController::setupNodeActions()
{
    foreach(QString kind, viewController->getNodeKinds()){
        RootAction* action = new RootAction("Node", kind);
        action->setIconPath("Items", kind);
        nodeKindActions[kind]= action;
        adoptableKindsGroup->addAction(action);
    }
}

void ToolbarController::setupEdgeActions()
{
    foreach(Edge::EDGE_KIND kind, EdgeFactory::getEdgeKinds()){
        QString edgeKind = EdgeFactory::getEdgeKindString(kind);
        RootAction* connectAction = new RootAction("Edge", edgeKind);
        RootAction* disconnectAction = new RootAction("Edge", edgeKind);
        connectAction->setIconPath("Items", edgeKind);
        disconnectAction->setIconPath("Items", edgeKind);
        connectEdgeKindActions[kind] = connectAction;
        disconnectEdgeKindActions[kind] = disconnectAction;
    }
}

NodeViewItemAction *ToolbarController::getNodeViewItemAction(int ID)
{
    if(actions.contains(ID)){
        return actions[ID];
    }
    return 0;
}

RootAction *ToolbarController::createRootAction(QString hashKey, QString actionName, QString iconPath, QString aliasPath)
{
    if(!toolActions.contains(hashKey)){
        RootAction* action = new RootAction("ToolActionController", actionName, this);
        action->setIconPath(iconPath, aliasPath);
        toolActions[hashKey] = action;
    }
    if(toolActions.contains(hashKey)){
        return toolActions[hashKey];
    }
    return 0;
}

