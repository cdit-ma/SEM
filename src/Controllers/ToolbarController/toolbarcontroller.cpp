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


    connect(Theme::theme(), SIGNAL(refresh_Icons()), this, SLOT(themeChanged()));

    connect(selectionController, &SelectionController::selectionChanged, this, &ToolbarController::selectionChanged);

    //Connect to the view controller
    connect(viewController, &ViewController::vc_actionFinished, this, &ToolbarController::actionFinished);
    connect(viewController, &ViewController::vc_viewItemConstructed, this, &ToolbarController::viewItem_Constructed);
    connect(viewController, &ViewController::vc_viewItemDestructing, this, &ToolbarController::viewItem_Destructed);


    themeChanged();
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

        if(!actions.contains(ID)){
            NodeViewItemAction* action = new NodeViewItemAction(node);
            viewController->getActionController()->updateIcon(action, Theme::theme());


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

                if (node->getNodeKind() == NODE_KIND::HARDWARE_NODE || node->getNodeKind() == NODE_KIND::HARDWARE_CLUSTER){
                    hardwareIDs.append(ID);
                    emit hardwareCreated(ID);
                }else if(node->getViewAspect() == VA_WORKERS && (node->getNodeKind() == NODE_KIND::WORKER_PROCESS || node->getNodeKind() == NODE_KIND::WORKLOAD)){
                    workerProcessIDs.append(ID);
                    emit workerProcessCreated(ID);
                }
                /*
                else if ((node->getNodeKind() == NODE_KIND::WORKER_PROCESS) || (node->getNodeKind() == NODE_KIND::WORKER_DEFINITIONS)) {
                                   if(node->getViewAspect() == VA_WORKERS){
                                       workerProcessIDs.append(ID);
                                       emit workerProcessCreated(ID);
                                   }
                               }*/
            }
        }
    }
}

void ToolbarController::viewItem_Destructed(int ID, ViewItem *)
{
    if (actions.contains(ID)){

        // TODO - Shouldn't the ID be removed from the list?
        if (hardwareIDs.contains(ID)){
            emit hardwareDestructed(ID);
        } else if (workerProcessIDs.contains(ID)) {
            emit workerProcessDestructed(ID);
        }

        NodeViewItemAction* action = actions[ID];
        actions.remove(ID);
        actionGroup->removeAction(action);
        action->deleteLater();
    }
}

void ToolbarController::selectionChanged(int selected)
{
    
    //Get the valid list of things to enable disable
    QList<NODE_KIND> validNodes = viewController->getAdoptableNodeKinds2();
    QList<EDGE_KIND> validEdges = viewController->getValidEdgeKindsForSelection();
    QList<EDGE_KIND> existingEdges = viewController->getExistingEdgeKindsForSelection();
    //Disable the
    foreach(RootAction* action, adoptableKindsGroup->getRootActions()){
        NodeViewItemAction* nodeAction = (NodeViewItemAction*) action;
        auto kind = nodeAction->getNodeKind();
        action->setEnabled(validNodes.contains(kind));
    }

    foreach(EDGE_KIND edgeKind, connectEdgeKindActions.keys()){
        RootAction* action = connectEdgeKindActions[edgeKind];
        action->setEnabled(validEdges.contains(edgeKind));
    }

    foreach(EDGE_KIND edgeKind, disconnectEdgeKindActions.keys()){
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

void ToolbarController::addChildNode(QString kind_str, QPointF position)
{
    ViewItem* item = selectionController->getActiveSelectedItem();

    if(item){
        int ID = item->getID();
        auto kind = EntityFactory::getNodeKind(kind_str);
        emit viewController->vc_constructNode(ID, kind, position);
    }
}

void ToolbarController::addEdge(int dstID, EDGE_KIND edgeKind)
{
    QList<int> IDs = selectionController->getSelectionIDs();
    if(!IDs.isEmpty()){
        emit viewController->vc_constructEdge(IDs, dstID, edgeKind);
    }
}

void ToolbarController::removeEdge(int dstID, EDGE_KIND edgeKind)
{
    QList<int> IDs = selectionController->getSelectionIDs();
    if(!IDs.isEmpty()){
        emit viewController->vc_destructEdges(IDs, dstID, edgeKind);
    }
}

void ToolbarController::addConnectedChildNode(int dstID, QString kind_str, QPointF position)
{
    int ID = selectionController->getFirstSelectedItemID();
    if(ID != -1){
        auto kind = EntityFactory::getNodeKind(kind_str);

        emit viewController->vc_constructConnectedNode(ID, kind,dstID, EDGE_KIND::NONE, position);
    }
}

void ToolbarController::addWorkerProcess(int processID, QPointF position)
{
    int ID = selectionController->getFirstSelectedItemID();
    if(ID != -1){
        emit viewController->vc_constructWorkerProcess(ID, processID, position);
    }
}

bool ToolbarController::requiresSubAction(NODE_KIND kind)
{
    return false;
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
    createRootAction("EC_DEPLOYMENT_CONNECT", "Deploy Selection", "Icons", "screen");
    createRootAction("EC_DEPLOYMENT_DISCONNECT", "Remove selection deployment", "Icons", "screenStriked");

    // setup menu info actions here
    createRootAction("INFO_NO_VALID_DEPLOYMENT_NODES", "There are no available hardware nodes", "Icons", "circleInfo");
    createRootAction("INFO_NO_UNIMPLEMENTED_COMPONENTS", "There are no IDL files containing unimplemented Component entities", "Icons", "circleInfo");
    createRootAction("INFO_NO_COMPONENTS", "There are no IDL files containing Component entities", "Icons", "circleInfo");
    createRootAction("INFO_NO_BLACKBOXES", "There are no IDL files containing BlackBox entities", "Icons", "circleInfo");
    createRootAction("INFO_NO_AGGREGATES", "There are no IDL files containing Aggregate entities", "Icons", "circleInfo");
    createRootAction("INFO_NO_VECTORS", "There are no IDL files containing initialised Vector entities", "Icons", "circleInfo");
    createRootAction("INFO_NO_FUNCTIONS", "There are no available functions", "Icons", "circleInfo");
    createRootAction("INFO_NO_OUTEVENTPORTS", "The selected entity's definition does not contain any OutEventPort entities", "Icons", "circleInfo");
    createRootAction("INFO_NO_VALID_EDGE", "There are no entities with the required kind to connect to", "Icons", "circleInfo");
    createRootAction("INFO_NO_EDGE_TO_DISCONNECT", "There are no edges to disconnect", "Icons", "circleInfo");
}

QList<NodeViewItemAction *> ToolbarController::getEdgeActionsOfKind(EDGE_KIND kind)
{
    QList<NodeViewItemAction*> list;

    foreach(ViewItem* item, viewController->getValidEdges(kind)){
        if(item && actions.contains(item->getID())){
            list.append(actions[item->getID()]);
        }
    }
    return list;
}

QList<NodeViewItemAction *> ToolbarController::getExistingEdgeActionsOfKind(EDGE_KIND kind)
{
    QList<NodeViewItemAction*> list;
    foreach(ViewItem* item, viewController->getExistingEdgeEndPointsForSelection(kind)){
        if(item && actions.contains(item->getID())){
            list.append(actions[item->getID()]);
        }
    }
    return list;
}

RootAction *ToolbarController::getConnectEdgeActionOfKind(EDGE_KIND kind)
{
    if(connectEdgeKindActions.contains(kind)){
        return connectEdgeKindActions[kind];
    }
    return 0;
}

RootAction *ToolbarController::getDisconnectEdgeActionOfKind(EDGE_KIND kind)
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
    auto theme = Theme::theme();
    foreach(auto node, viewController->getNodeKindItems()){
        auto nodeKind = node->getNodeKind();

        NodeViewItemAction* action = new NodeViewItemAction(node);
        viewController->getActionController()->updateIcon(action, theme);


        nodeKindActions[nodeKind]= action;
        adoptableKindsGroup->addAction(action);
    }
}

void ToolbarController::setupEdgeActions()
{
    foreach(EDGE_KIND kind, EntityFactory::getEdgeKinds()){
        QString edgeKind = EntityFactory::getEdgeKindString(kind);
        RootAction* connectAction = new RootAction("Edge", edgeKind);
        RootAction* disconnectAction = new RootAction("Edge", edgeKind);
        connectAction->setIconPath("EntityIcons", edgeKind);
        disconnectAction->setIconPath("EntityIcons", edgeKind);
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

