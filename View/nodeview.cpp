#include "nodeview.h"
#include "../Controller/controller.h"
#include "Toolbar/toolbarwidget.h"
#include "Dock/docktogglebutton.h"
#include "../medeasubwindow.h"
#include <limits>


#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QTouchEvent>
#include <QRect>
#include <QDebug>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include <QAction>
#include <QVBoxLayout>
#include <QTime>
#include <QTableView>
#include <cmath>

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR

#define MAX_ZOOM_RATIO 50
#define MIN_ZOOM_RATIO 2

#define VIEW_PADDING 1.25


/**
 * @brief NodeView::NodeView
 * @param subView
 * @param parent
 */
NodeView::NodeView(bool subView, QWidget *parent):QGraphicsView(parent)
{
    wasPanning = false;
    centralizedNodeItem = 0;
    constructedFromImport = true;
    toolbarJustClosed = false;
    editingNodeItemLabel = false;
    managementComponentVisible = false;
    toolbarDockConstruction = false;
    importFromJenkins = false;
    hardwareDockOpen = false;

    IS_SUB_VIEW = subView;




    //controller = 0;
    parentNodeView = 0;
    rubberBand = 0;

    clearingModel = false;
    updateDeployment = false;

    prevLockMenuOpened = 0;

    CENTRALIZED_ON_ITEM = false;
    MINIMAP_EVENT = false;


    AUTO_CENTER_ASPECTS = false;
    GRID_LINES_ON = false;
    SELECT_ON_CONSTRUCTION = false;
    CONTROL_DOWN = false;
    SHIFT_DOWN = false;
    IS_DESTRUCTING = false;

    pasting = false;


    viewState = VS_NONE;

    eventFromEdgeItem = false;



    MINIMAP_EVENT = false;
    setScene(new QGraphicsScene(this));

    //Set QT Options for this QGraphicsView
    setDragMode(NoDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);



    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Set GraphicsView background-color
    setStyleSheet("QGraphicsView{ background-color: rgba(170,170,170,255); border: 0px;}");


    //Set The rubberband Mode.
    setRubberBandMode(false);

    //Setup Aspects
    allAspects << "Assembly";
    allAspects << "Hardware";
    allAspects << "Definitions";
    allAspects << "Workload";

    defaultAspects << "Definitions";
    //defaultAspects << "Assembly";
    //defaultAspects << "Hardware";

    nonDrawnItemKinds << "DeploymentDefinitions";

    //create toolbar widget
    toolbar = new ToolbarWidget(this);
    connect(this, SIGNAL(view_updateMenuActionEnabled(QString,bool)), toolbar, SLOT(updateActionEnabled(QString, bool)));

    // initialise the view's center point
    centerPoint = getVisibleRect().center();

    currentMapKey = -1;
    initialRect = 0;
    viewMovedBackForward = false;

    numberOfNotifications = 1;
    notificationNumber = 0;

}


void NodeView::setController(NewController *c)
{
    controller = c;
}

void NodeView::disconnectController()
{
    controller = 0;
}

NodeView::VIEW_STATE NodeView::getViewState()
{
    return viewState;
}


void NodeView::ensureAspect(QString ID)
{
    NodeItem* nodeItem = getNodeItemFromID(ID);
    if(nodeItem){
        Node* node = nodeItem->getNode();
        if(node){
            QStringList newAspects = currentAspects;

            if(node->isDefinition()){
                newAspects.append("Definitions");
            }
            if(node->isImpl()){
                newAspects.append("Behaviour");
            }
            if(node->isInstance()){
                newAspects.append("Assembly");
            }

            newAspects.removeDuplicates();
            setAspects(newAspects);

        }
    }
}

bool NodeView::isSubView()
{
    return IS_SUB_VIEW;
}

bool NodeView::isTerminating()
{
    return IS_DESTRUCTING;
}

bool NodeView::isNodeKindDeployable(QString nodeKind)
{
    if (nodeKind == "ComponentAssembly" || nodeKind == "ComponentInstance" || nodeKind == "ManagementComponent" || nodeKind == "BlackBoxInstance") {
        return true;
    }
    return false;
}

bool NodeView::isMainView()
{
    return !IS_SUB_VIEW;
}

void NodeView::addAspect(QString aspect)
{
    if(!currentAspects.contains(aspect)){
        QStringList newAspects = currentAspects;
        newAspects.append(aspect);
        setAspects(newAspects);
    }
}

void NodeView::removeAspect(QString aspect)
{
    if(currentAspects.contains(aspect)){
        QStringList newAspects = currentAspects;
        newAspects.removeAll(aspect);
        setAspects(newAspects);
    }
}

NodeView::~NodeView()
{
    IS_DESTRUCTING = true;
    //Clear the current Selected Attribute Model so that the GUI doesn't crash.
    setAttributeModel(0, true);

    if(parentNodeView && !parentNodeView->isTerminating()){
        parentNodeView->removeSubView(this);

    }
}

void NodeView::destroySubViews()
{
    while(!subViews.isEmpty()){
        subViews.takeFirst()->deleteLater();
        //delete subViews.first();
    }
}


/**
 * @brief NodeView::getVisibleRect
 * This returns the current rectangle visualised by this view in scene coordinates.
 * @return
 */
QRectF NodeView::getVisibleRect()
{
    QPointF topLeft = mapToScene(0,0);
    QPointF bottomRight = mapToScene(viewport()->width(), viewport()->height());
    return QRectF( topLeft, bottomRight );
}


/**
 * @brief NodeView::centerRect
 * This scales and centers the view based on the provided rectangle.
 * @param rect - rectangle to center the view on
 * @param padding - padding around the rectangle
 * @param addToMap - determines whether to add the rect/pos to the maps
 * @param sizeRatio - determines how much the rect should be scaled
 */
void NodeView::centerRect(QRectF rect, double padding, bool addToMap, double sizeRatio)
{
    // store rect's original center
    QPointF rectCenter = rect.center();

    // this adds a rect/pos to the maps used by the view to move backwards & forwards
    if (addToMap) {
        if (initialRect < 1) {
            // this is a temporary fix to keeping the maps cleared until
            // the user manually changes the view's centered rectangle
            initialRect++;
            clearMaps();
        } else {
            if (viewMovedBackForward) {
                clearMaps(currentMapKey + 1);
                viewMovedBackForward = false;
            }
            addToMaps(getModelScenePos(), rect);
        }
    }

    // check if there is a specified value for padding
    if (padding == 0) {
        padding = VIEW_PADDING;
    }

    // add the padding to the rect to be centered
    rect.setWidth(rect.width()*padding);
    rect.setHeight(rect.height()*padding);

    // calculate the ratio - viewport : rect
    // scale depending on which side requires less scaling
    double widthScale = viewport()->width() / rect.width() * sizeRatio;
    double heightScale = viewport()->height() / rect.height() * sizeRatio;
    double newScale = qMin(widthScale, heightScale);

    // reset current transform before scaling
    setTransform(QTransform());
    scale(newScale, newScale);

    // center the view on rect's original center
    centerViewOn(rectCenter);
}



/**
 * @brief NodeView::centerViewOn
 * This centers the view by translating the model item to move the
 * provided center point to the current viewport's center point.
 * @param center
 */
void NodeView::centerViewOn(QPointF center)
{
    QPointF deltaPos = getVisibleRect().center() - center;
    adjustModelPosition(deltaPos);
    updateViewCenterPoint();
}


/**
 * @brief NodeView::recenterView
 * This recenters the view based on the provided model position.
 * It calculates where the view was centered on the model based on modelPos and
 * then translates the model so that the view is once again centered on that same point.
 * @param modelPos - model's position on the previous centerering of the view
 * @param centeredRect - the view's centered rect when the model was at modelPos
 */
void NodeView::recenterView(QPointF modelPos, QRectF centeredRect, bool addToMap)
{
    QPointF prevDeltaPos = getVisibleRect().center() - modelPos;
    QPointF currentDeltaPos = getVisibleRect().center() - getModelScenePos();
    QPointF deltaPos = currentDeltaPos - prevDeltaPos;

    adjustModelPosition(deltaPos);
    updateViewCenterPoint();

    // after translating the model, zoom to fit on the rect
    centerRect(centeredRect, 0, addToMap);
}


/**
 * @brief NodeView::getModelItem
 * This returns the NodeItem for the model.
 * @return
 */
NodeItem* NodeView::getModelItem()
{
    //MODEL ALWAYS AT 1, WE think
    QString ID = "";
    if(controller){
        ID = controller->getModel()->getID();
    }
    return getNodeItemFromID(ID);
}


/**
 * @brief NodeView::getModelScenePos
 * This returns the model item's scene position.
 * @return
 */
QPointF NodeView::getModelScenePos()
{
    if (getModelItem()) {
        return getModelItem()->scenePos();
    }
    qWarning() << "NodeView::getModelScenePos - There is no model item.";
    return QPointF();
}

/**
 * @brief NodeView::adjustModelPosition When the model position is changed, we need to translate all of the edges.
 * @param delta
 */
void NodeView::adjustModelPosition(QPointF delta)
{
    if(getModelItem()){
        getModelItem()->adjustPos(delta);
    }
}


/**
 * @brief NodeView::getMapSize
 * This returns the size of both modelPositions and viewCeneteredrects.
 * These maps should have identical size and keys.
 * @return
 */
int NodeView::getMapSize()
{
    return modelPositions.size();
}


/**
 * @brief NodeView::addToMaps
 * This adds the current model position and centered rectangle
 * to the maps used to move the view backwards and forwards.
 * @param modelPos - model position
 * @param centeredRect - centered rectangle
 */
void NodeView::addToMaps(QPointF modelPos, QRectF centeredRect)
{
    int key = getMapSize();
    modelPositions[key] = modelPos;
    centeredRects[key] = centeredRect;
}


/**
 * @brief NodeView::clearMaps
 * This clears the maps used to move the view backwards and forwards
 * from the provided key, and resets the key used to navigate them.
 * @param fromKey -key to start clearing from
 */
void NodeView::clearMaps(int fromKey)
{
    int mapSize = getMapSize();
    for (int i = fromKey; i < mapSize; i++) {
        modelPositions.remove(i);
        centeredRects.remove(i);
    }
    currentMapKey = -1;
}


/**
 * @brief NodeView::getNodeItemsList
 * Return the list of all node items on the scene.
 * @return
 */
QList<NodeItem*> NodeView::getNodeItemsList()
{
    QList<NodeItem*> nodeItems;
    foreach (GraphMLItem* item, guiItems) {
        if (item->isNodeItem()) {
            nodeItems.append((NodeItem*)item);
        }
    }
    return nodeItems;
}

QList<EdgeItem *> NodeView::getEdgeItemsList()
{
    QList<EdgeItem*> edgeItems;
    foreach (GraphMLItem* item, guiItems) {
        if (item->isEdgeItem()) {
            edgeItems.append((EdgeItem*)item);
        }
    }
    return edgeItems;
}


/**
 * @brief NodeView::allowedFocus
 * This returns whether the widget in focus should allow the
 * selected node to be deleted when delete is pressed.
 * @param widget
 * @return
 */
bool NodeView::allowedFocus(QWidget *widget)
{
    // the view has focus but the focus is on the editable text item
    if (this->hasFocus() && editingNodeItemLabel) {
        return false;
    }

    // the data table has focus
    QTableView* tv = dynamic_cast<QTableView*>(widget);
    if (tv) {
        return false;
    }

    // either the search bar or the expanding line edit on the data table has focus
    QLineEdit* le = dynamic_cast<QLineEdit*>(widget);
    if (le) {
        return false;
    }

    return true;
}


/**
 * @brief NodeView::updateDisplayedChildrenNodes
 * @param viewMode
 */
void NodeView::updateDisplayedChildrenNodes(int viewMode)
{
    foreach (QString ID, selectedIDs) {
        NodeItem* item = getNodeItemFromID(ID);
        if (item) {
            item->updateChildrenViewMode(viewMode);
        }
    }
}


/**
 * @brief NodeView::getSelectedNode
 * This returns the currently selected node.
 * If there are multiple nodes selected, it returns null.
 * @return
 */
Node* NodeView::getSelectedNode()
{
    NodeItem* nodeItem = getSelectedNodeItem();
    if(nodeItem){
        return nodeItem->getNode();
    }
    return 0;
}


QString NodeView::getSelectedNodeID()
{
    if (selectedIDs.size() == 1) {
        QString ID = selectedIDs[0];
        GraphMLItem* item = getGraphMLItemFromHash(ID);
        if (item && item->isNodeItem()) {
            return item->getID();
        }
    }
    return "";
}


/**
 * @brief NodeView::getSelectedNodeItem
 * This returns the currently selected node item based on getSelectedNode().
 * @return
 */
NodeItem* NodeView::getSelectedNodeItem()
{
    QString ID = getSelectedNodeID();
    if(ID != ""){
        return (NodeItem*) getGraphMLItemFromHash(ID);
    }
    return 0;
}


/**
 * @brief NodeView::getSelectedNodeItems
 * @return
 */
QList<NodeItem*> NodeView::getSelectedNodeItems()
{
    QList<NodeItem*> selectedItems;
    foreach (QString ID, selectedIDs) {
        NodeItem* selectedItem = getNodeItemFromID(ID);
        if (selectedItem) {
            selectedItems.append(selectedItem);
        }
    }
    return selectedItems;
}

QStringList NodeView::getSelectedNodeIDs()
{

    QStringList selectedItems;
    foreach (QString ID, selectedIDs) {
        NodeItem* selectedItem = getNodeItemFromID(ID);
        if (selectedItem) {
            selectedItems.append(ID);
        }
    }
    return selectedItems;
}


void NodeView::setParentNodeView(NodeView *n)
{
    this->parentNodeView = n;
}

void NodeView::removeSubView(NodeView *subView)
{
    if(subViews.contains(subView)){
        subViews.removeAll(subView);
    }
}


QList<GraphMLItem *> NodeView::search(QString searchString, GraphMLItem::GUI_KIND kind)
{
    QList<GraphMLItem*> returnable;

    foreach(GraphMLItem* guiItem, guiItems){
        GraphML* gml = guiItem->getGraphML();
        if((kind == GraphMLItem::NODE_ITEM && guiItem->isNodeItem()) || (kind == GraphMLItem::NODE_EDGE && guiItem->isEdgeItem())){
            if(gml->getDataValue("label").toLower().contains(searchString.toLower())){
                returnable.append(guiItem);
            }
        }
    }

    return returnable;
}


QStringList NodeView::getAdoptableNodeList(QString ID)
{
    return controller->getAdoptableNodeKinds(ID);
}


QStringList NodeView::getConnectableNodes(QString ID)
{
    return controller->getConnectableNodes(ID);
}

QList<NodeItem *> NodeView::getConnectableNodeItems(QString ID)
{
    QList<NodeItem*> nodeItems;
    QStringList IDs = controller->getConnectableNodes(ID);
    NodeItem* src = getNodeItemFromID(ID);
    foreach(QString cID, IDs){
        NodeItem* nodeItem = getNodeItemFromID(cID);

        if(src && nodeItem){
            //Ignore edges between AggregateInstance and AggregateInstances in Dock
            if(src->getNodeKind() == "AggregateInstance" && nodeItem->getNodeKind() == "AggregateInstance"){
                continue;
            }
        }
        if(nodeItem){
            nodeItems.append(nodeItem);
        }
    }
    return nodeItems;
}


/**
 * @brief NodeView::getConnectableNodeItems
 * @param IDs
 * @return
 */
QList<NodeItem*> NodeView::getConnectableNodeItems(QStringList IDs)
{
    Q_UNUSED(IDs);
    /*
    if (IDs.isEmpty()) {
        IDs = selectedIDs;
    }

    QHash<QString, int> connectableNodesHash;
    foreach (QString ID, IDs) {
        QStringList connectableNodes = getConnectableNodes(ID);
        connectableNodes.removeDuplicates();
        foreach (QString cnID, connectableNodes) {
            connectableNodesHash[cnID] = connectableNodesHash[cnID]++;
        }
    }


    QList<NodeItem*> sharedLegalNodeItems;
    foreach (QString hashID, connectableNodesHash.keys()) {
        if (connectableNodesHash[hashID] == IDs.count()) {
            NodeItem* item = getNodeItemFromID(hashID);
            if (item) {
                sharedLegalNodeItems.append(item);
            }
        }
    }

    return sharedLegalNodeItems;
    */

    return QList<NodeItem*>();
}


QList<NodeItem *> NodeView::getNodeInstances(QString ID)
{
    QList<NodeItem*> nodeItems;
    if(controller){
        foreach(QString iID, controller->getInstances(ID)){
            NodeItem* nodeItem = getNodeItemFromID(iID);
            if(nodeItem){
                nodeItems.append(nodeItem);
            }
        }
    }
    return nodeItems;
}


void NodeView::constructNewView(QString nodeID)
{
    if(nodeID == ""){
        nodeID = getSelectedNodeID();
    }

    if (nodeID == "") {
        emit view_displayNotification("Select an entity first");
        return;
    }

    NodeItem* nodeItem = getNodeItemFromID(nodeID);

    if(IS_SUB_VIEW || nodeID == "" || !nodeItem){
        return;
    }


    MedeaSubWindow* subWindow = new MedeaSubWindow();

    connect(this, SIGNAL(destroyed()), subWindow, SLOT(reject()));

    NodeView* newView = new NodeView(true, subWindow);

    subViews.append(newView);

    newView->setAspects(allAspects);
    newView->setParentNodeView(this);

    subWindow->setNodeView(newView);




    Node* currentNode = nodeItem->getNode();
    if(controller && currentNode ){
        controller->connectView(newView);


        QList<Edge*> edgeList;
        QList<Node*> constructList;
        //Get the Children
        constructList << currentNode->getChildren();
        edgeList << currentNode->getEdges();

        while(currentNode){
            //Add the Parent of the currentNode
            constructList.insert(0,currentNode);
            currentNode = currentNode->getParentNode();
        }

        while(!constructList.isEmpty()){
            newView->constructGUIItem(constructList.takeFirst());
        }
        while(!edgeList.isEmpty()){
            newView->constructGUIItem(edgeList.takeFirst());
        }

        newView->view_LockCenteredGraphML(nodeID);
        connect(this, SIGNAL(view_ClearSubViewAttributeTable()), newView, SIGNAL(view_ClearSubViewAttributeTable()));
        subWindow->show();
        newView->view_LockCenteredGraphML(nodeID);
    }else{
        delete subWindow;
    }
}

QList<NodeItem *> NodeView::getNodeItemsOfKind(QString kind, QString ID, int depth)
{
    QList<NodeItem*> nodes;

    if(controller){
        foreach(QString childID, controller->getNodesOfKind(kind, ID, depth)){
            NodeItem* child = getNodeItemFromID(childID);
            if(child){
                nodes.append(child);
            }
        }
    }
    return nodes;
}


bool NodeView::viewportEvent(QEvent * e)
{
    emit view_ViewportRectChanged(getVisibleRect());
    return QGraphicsView::viewportEvent(e);
}

/**
 * @brief NodeView::actionFinished Called when the Controller has finished running an Action
 */
void NodeView::actionFinished()
{
    // not sure if this is needed here! - reset notification seq and total number
    numberOfNotifications = 1;
    notificationNumber = 0;

    //Reset
    pasting = false;
    toolbarDockConstruction = false;

    if (constructedFromImport) {
        // added this here to re-centralise on the model after import
        fitToScreen();
    }
    constructedFromImport = false;

    if (importFromJenkins) {
        emit view_OpenHardwareDock();
        importFromJenkins = false;
    }

    if (clearingModel) {
        resetModel(false);
        clearingModel = false;
    }

    if(updateDeployment){
        edgeConstructed_signalUpdates();
        updateDeployment = false;
    }

    updateActionsEnabled();
    enableClipboardActions();

    viewMutex.unlock();
}


void NodeView::setStateResizing()
{
    setState(VS_RESIZING);
}

void NodeView::setStateMove()
{
    setState(VS_SELECTED);
}

void NodeView::setStateMoving()
{
    setState(VS_MOVING);
}

void NodeView::setStateSelected()
{
    setState(VS_SELECTED);
}

void NodeView::request_ImportSnippet()
{
    NodeItem* nodeItem = this->getSelectedNodeItem();
    if (nodeItem) {
        emit view_ImportSnippet(nodeItem->getNodeKind());
    }
}

void NodeView::hardwareDockOpened(bool opened)
{
    this->hardwareDockOpen = opened;
    if(opened){
        highlightDeployment();
    }else{
        highlightDeployment(true);
    }
}



void NodeView::showQuestion(MESSAGE_TYPE type, QString title, QString message, QString ID)
{
    Q_UNUSED(type);
    Q_UNUSED(ID);
    int reply = QMessageBox::question(this, title, message, QMessageBox::Yes | QMessageBox::No);
    bool yes = reply == QMessageBox::Yes;
    emit view_QuestionAnswered(yes);
}

void NodeView::setAttributeModel(GraphMLItem *item, bool tellSubView)
{
    if(item){
        if(currentTableID != item->getID()){
            currentTableID = item->getID();
            emit view_SetAttributeModel(item->getAttributeTable());
        }
    }else{
        currentTableID = "";
        emit view_SetAttributeModel(0);
        if(!IS_SUB_VIEW && tellSubView){
            emit view_ClearSubViewAttributeTable();
        }
    }
}

void NodeView::importProjects(QStringList xmlDataList)
{
    if(!xmlDataList.isEmpty()){
        if(viewMutex.tryLock()){
            constructedFromImport = true;
            clearSelection();
            emit view_ImportProjects(xmlDataList);
        }
    }
}

void NodeView::loadJenkinsNodes(QString fileData)
{
    importFromJenkins = true;
    viewDeploymentAspect();
    importProjects(QStringList(fileData));
}

void NodeView::exportSnippet()
{
    if(viewMutex.tryLock()){
        emit view_ExportSnippet(selectedIDs);
    }
}

void NodeView::exportProject()
{
    if(viewMutex.tryLock()){
        emit view_ExportProject();
    }
}

void NodeView::importSnippet(QString fileName, QString fileData)
{
    if(viewMutex.tryLock()){
        pasting = true;
        //Clear before pasting
        QStringList duplicateList = selectedIDs;
        clearSelection();
        emit view_ImportedSnippet(duplicateList, fileName, fileData);
    }
}

void NodeView::minimapPressed(QMouseEvent *event)
{
    MINIMAP_EVENT = true;
    mousePressEvent(event);
    //MINIMAP_EVENT = false;
}

void NodeView::minimapReleased(QMouseEvent *event)
{
    //MINIMAP_EVENT = true;
    mouseReleaseEvent(event);
    MINIMAP_EVENT = false;

    // MINIMAP_EVENT = false;

}

void NodeView::minimapMoved(QMouseEvent *event)
{
    //MINIMAP_EVENT = true;
    mouseMoveEvent(event);
    //MINIMAP_EVENT = false;
}


void NodeView::scrollEvent(int delta)
{
    QRectF viewRect = viewport()->rect();
    QRectF scaledSceneRect(QPointF(0,0), sceneRect().size()*transform().m11());

    if (delta > 0) {
        // zoom in - maximum scale is when the scene is 50 times the size of the view
        if (viewRect.width()*MAX_ZOOM_RATIO > scaledSceneRect.width()) {
            scale(ZOOM_SCALE_INCREMENTOR, ZOOM_SCALE_INCREMENTOR);
        }
    } else if (delta < 0) {
        // zoom out - minimum scale is when the view is twice the size of the scene
        if (viewRect.width() < scaledSceneRect.width()*MIN_ZOOM_RATIO) {
            scale(ZOOM_SCALE_DECREMENTOR, ZOOM_SCALE_DECREMENTOR);
        }
    }
}




void NodeView::minimapPan(QPointF delta)
{
    //QRectF rectangle = getVisibleRect();
    //rectangle.translate(delta);

    //getModelItem()->adjustPos(delta);
    //updateViewCenterPoint();

    //centerViewOn(getVisibleRect().center() + delta);
    //qCritical() << "Before: " << getVisibleRect().center();

    ViewportAnchor currentAnchor = transformationAnchor();
    setTransformationAnchor(NoAnchor);
    translate(-delta.x(), -delta.y());
    setTransformationAnchor(currentAnchor);

    //qCritical() << "After: "  << getVisibleRect().center();

    //*/
}


void NodeView::setRubberBandMode(bool On)
{
    if(isMainView()){
        if(!rubberBand){
            rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        }
        if(On){
            setDragMode(RubberBandDrag);
        }else{
            setDragMode(NoDrag);
            rubberBand->setVisible(false);
        }
    }
}


/**
 * @brief NodeView::clearView
 * This is called when the a new project is created. It clears the scene.
 */
void NodeView::clearView()
{
    //scene()->clear();
    //viewport()->update();
}


/**
 * @brief NodeView::triggerAction
 * This adds an action to the stack along with the current model's
 * position and the current centered rectangle in the view.
 * @param action
 */
void NodeView::triggerAction(QString action)
{
    int mapKey = getMapSize() - 1;
    if (mapKey >= 0) {
        viewModelPositions.append(modelPositions[mapKey]);
        viewCenteredRectangles.append(centeredRects[mapKey]);
    }

    view_TriggerAction(action);
}


/**
 * @brief NodeView::setAspects
 * This is called whenever the view aspect buttons are clicked.
 * @param aspects
 * @param centerViewAspects
 */
void NodeView::setAspects(QStringList aspects, bool centerViewAspects)
{
    currentAspects = aspects;

    // set change in view aspects to the node items and the gui
    view_AspectsChanged(aspects);
    view_GUIAspectChanged(aspects);

    if (!IS_SUB_VIEW && AUTO_CENTER_ASPECTS && centerViewAspects) {
        fitToScreen();
    }

    // only need to clear the selection if any of the selected items is now not in aspect
    foreach (QString id, selectedIDs) {
        NodeItem* item = (NodeItem*)guiItems[id];
        if (!item->isInAspect()) {
            clearSelection();
            break;
        }
    }


}


/**
 * @brief NodeView::sortNode
 * This method recursively sorts the provided node.
 * It sorts from the lowest level children back up to the provided node.
 * @param node
 * @param topMostNode
 */
void NodeView::sortSelection()
{

    if(viewMutex.tryLock()){
        triggerAction("View: Sorting Node");
        emit view_updateProgressStatus(-1, "Sorting Selected Entities");

        foreach(QString ID, selectedIDs){
            NodeItem* nodeItem = getNodeItemFromID(ID);
            if(nodeItem){
                nodeItem->newSort();
            }
        }
        emit view_updateProgressStatus(100);

        actionFinished();
    }


}


/**
 * @brief NodeView::centerAspect
 * @param aspect
 */
void NodeView::centerAspect(QString aspect)
{
    if (controller) {

        Node* model = controller->getModel();
        Node* aspectNode = 0;

        if (model) {
            if (aspect == "Definitions") {
                aspect = "Interface";
            } else if (aspect == "Workload") {
                aspect = "Behaviour";
            }
            aspect += "Definitions";
            aspectNode = model->getChildrenOfKind(aspect).at(0);
        }

        if (aspectNode) {
            centerOnItem(getNodeItemFromNode(aspectNode));
        }
    }
}


/**
 * @brief NodeView::sortEntireModel
 * This recursively sorts the enitre model.
 */
void NodeView::sortEntireModel()
{
    sortSelection();
}


/**
 * @brief NodeView::centerItem
 * This scales and translates the view to fit and center on the item.
 * @param item - graphics item to center on
 */
void NodeView::centerItem(GraphMLItem *item)
{
    if(!item){
        item = getSelectedNodeItem();
    }
    if (item) {
        centerRect(item->sceneBoundingRect());
    }
}

void NodeView::centralizedItemMoved()
{
    if(CENTRALIZED_ON_ITEM){
        centerItem(centralizedItemID);
    }
}

void NodeView::centerItem(QString ID)
{
    GraphMLItem* item = getGraphMLItemFromHash(ID);
    centerItem(item);
}

void NodeView::centerDefinition(QString ID)
{
    if(controller){
        QString definitionID =  controller->getDefinition(ID);

        GraphMLItem *definition = getNodeItemFromID(definitionID);

        if(definition){
            // make sure the Assembly view aspect is on
            addAspect("Definitions");
            clearSelection(false);
            appendToSelection(definition);
            centerOnItem();
        }
    }
}

void NodeView::centerImplementation(QString ID)
{
    if(controller){
        QString implementationID =  controller->getImplementation(ID);
        GraphMLItem *impl = getNodeItemFromID(implementationID);

        if(impl){
            // make sure the Assembly view aspect is on
            addAspect("Workload");
            clearSelection(false);
            appendToSelection(impl);
            centerOnItem();
        }
    }

}

void NodeView::centerInstance(QString instanceID)
{
    if(controller){
        GraphMLItem *instance = getNodeItemFromID(instanceID);

        if(instance){
            // make sure the Assembly view aspect is on
            addAspect("Assembly");
            clearSelection(false);
            appendToSelection(instance);
            centerOnItem();
        }
    }

}


/**
 * @brief NodeView::centerOnItem
 * This centers on the selected node and zooms in/out enough so that the node
 * is roughly one fourth of the set minimum window/view height.
 * @param item - graphics item to center on
 */
void NodeView::centerOnItem(GraphMLItem* item)
{
    NodeItem* nodeItem = 0;

    if (item) {
        nodeItem = qobject_cast<NodeItem*>(item);
    } else {
        nodeItem = getSelectedNodeItem();
    }

    if (nodeItem) {

        // if the selected node is a main container, just use centerItem()
        // we would only ever want to center and zoom into it
        QString nodeKind = nodeItem->getNodeKind();
        if (nodeKind == "Model" || nodeKind.endsWith("Definitions")) {
            centerItem(nodeItem);
            return;
        }

        // set the centralised height to be 1/4 of the window height
        centerRect(nodeItem->sceneBoundingRect(), 0, true, 0.25);

    } else {
        view_displayNotification("Select entity to center on.");
    }
}


/**
 * @brief NodeView::editableItemHasFocus
 * This tells the view whether the user is currently editing a node item's label or not.
 * @param hasFocus
 */
void NodeView::editableItemHasFocus(bool hasFocus)
{
    editingNodeItemLabel = hasFocus;
}


/**
 * @brief NodeView::selectAndCenter
 * This selects and center on the provided item.
 * If an ID is provided instead of an item, get the corresponding item from the hash.
 * @param item - graphics item to select and center on
 * @param ID - ID of the graphics item to select and center on
 */
void NodeView::selectAndCenter(GraphMLItem* item, QString ID)
{
    if (!item) {
        item = getGraphMLItemFromHash(ID);
    }

    if (item && item->isNodeItem()) {

        NodeItem* nodeItem = (NodeItem*) item;

        // make sure the view aspect the the item belongs to is turned on
        QStringList neededAspects = currentAspects;
        foreach (QString aspect, nodeItem->getAspects()) {
            if (!currentAspects.contains(aspect)) {
                neededAspects.append(aspect);
            }
        }

        // update view aspects
        setAspects(neededAspects, false);

        // make sure that the parent of nodeItem (if there is one) is expanded
        NodeItem* parentItem = nodeItem->getParentNodeItem();
        if (parentItem && !parentItem->isExpanded()) {
            parentItem->setNodeExpanded(true);
        }

        // clear the selection, select the item and then center on it
        clearSelection();
        appendToSelection(nodeItem->getNode());
        centerOnItem(item);

    } else {
        view_displayNotification("Entity no longer exists!");
    }
}



/**
 * @brief NodeView::keepSelectionFullyVisible
 * This is called whenever a node item is moved or resized.
 * If the item is selected, adjust the view rect accordingly to
 * fit the whole item then center on the adjusted view rect.
 * @param item - graphics item that was changed
 */
void NodeView::keepSelectionFullyVisible(GraphMLItem* item, bool sizeChanged)
{
    // turning this off for now - need to re-access where and whether this function is useful
    return;

    if (!item || !selectedIDs.contains(item->getID())) {
        return;
    }

    if (item->getGraphML()->getDataValue("kind") == "Model") {
        return;
    }

    QRectF origViewRect = getVisibleRect();
    QRectF viewRect = origViewRect;
    QRectF itemRect = item->sceneBoundingRect();

    // translate the view rect to fit the selected item(s)
    // we only really need to check the bottom right corner
    // of the view if the item has increased in size
    if (itemRect.top() < viewRect.top() && !sizeChanged) {
        viewRect.translate(0, itemRect.top() - viewRect.top());
    }
    if (itemRect.bottom() > viewRect.bottom()) {
        viewRect.translate(0, itemRect.bottom() - viewRect.bottom());
    }
    if (itemRect.left() < viewRect.left() && !sizeChanged) {
        viewRect.translate(itemRect.left() - viewRect.left(), 0);
    }
    if (itemRect.right() > viewRect.right()) {
        viewRect.translate(itemRect.right() - viewRect.right(), 0);
    }

    // if the view rect was changed, recenter on it
    if (viewRect != origViewRect) {
        centerRect(viewRect, 1.025, false);
    }
}


/**
 * @brief NodeView::moveViewBack
 * This moves the view back to where it was previously centered.
 */
void NodeView::moveViewBack()
{
    if (!centeredRects.isEmpty()) {
        // set the initial value to be at the end of the map
        if (currentMapKey == -1) {
            currentMapKey = getMapSize() - 1;
        }
        if (currentMapKey > 0) {
            currentMapKey--;
            recenterView(modelPositions[currentMapKey], centeredRects[currentMapKey]);
            viewMovedBackForward = true;
        } else {
            //qWarning() << "NodeView::moveViewBack - Can't go back any further";
        }
    }
}


/**
 * @brief NodeView::moveViewForward
 * This moves the view forward to where it was previously centered.
 */
void NodeView::moveViewForward()
{
    if (!centeredRects.isEmpty()) {
        // set the initial value to be at the end of the map
        if (currentMapKey == -1) {
            currentMapKey = getMapSize() - 1;
        }
        if (currentMapKey < getMapSize() - 1) {
            currentMapKey++;
            recenterView(modelPositions[currentMapKey], centeredRects[currentMapKey]);
            viewMovedBackForward = true;
        } else {
            //qWarning() << "NodeView::moveViewForward - Can't go forward any further";
        }
    }
}


/**
 * @brief NodeView::highlightDeployment
 * @param clear
 */
void NodeView::highlightDeployment(bool clear)
{
    // clear highlighted node items
    if (guiItems.contains(prevSelectedNodeID)) {
        GraphMLItem* item = guiItems[prevSelectedNodeID];
        if (item->isNodeItem()) {
            ((NodeItem*)item)->deploymentView(false);
        }
    }

    // check if any ComponentAssemblies, ComponentInstances or ManagementComponents
    // have children deployed to a different node; show red hardware icon
    if (controller) {
        Model* model = controller->getModel();
        if (model) {
            foreach (Node* assm, model->getChildrenOfKind("ComponentAssembly")) {
                NodeItem* assmItem = getNodeItemFromNode(assm);
                if (assmItem) {
                    assmItem->deploymentView(true && !clear);
                }
            }
            foreach (Node* inst, model->getChildrenOfKind("ComponentInstance")) {
                NodeItem* instItem = getNodeItemFromNode(inst);
                if (instItem) {
                    instItem->deploymentView(true && !clear);
                }
            }
            foreach (Node* mngt, model->getChildrenOfKind("ManagementComponent")) {
                NodeItem* mngtItem = getNodeItemFromNode(mngt);
                if (mngtItem) {
                    mngtItem->deploymentView(true && !clear);
                }
            }
        }
    }

    if (clear) {
        prevSelectedNodeID = "";
        return;
    }

    NodeItem* selectedNodeItem = getSelectedNodeItem();
    if (selectedNodeItem) {
        if (selectedNodeItem->getNodeKind().startsWith("Hardware")) {
            return;
        }
        if (!selectedNodeItem->deploymentView(true, selectedNodeItem).isEmpty()) {
            // if there are higlighted children, display notification
            view_displayNotification("The selected entity has children that are deployed to a different node.");
        }
        prevSelectedNodeID = selectedNodeItem->getID();
    }
}


/**
 * @brief NodeView::setEventFromEdgeItem
 */
void NodeView::setEventFromEdgeItem()
{
    eventFromEdgeItem = true;
}


/**
 * @brief NodeView::showHardwareClusterChildrenViewMenu
 * @param nodeItem
 */
void NodeView::showHardwareClusterChildrenViewMenu(NodeItem *nodeItem)
{
    QMenu* menu = nodeItem->getChildrenViewOptionMenu();
    if (menu) {
        if (menu == prevLockMenuOpened) {
            menu->close();
            prevLockMenuOpened = 0;
        } else {
            QRectF lockRect = nodeItem->geChildrenViewOptionMenuSceneRect();
            QPoint offset(lockRect.width()/5, -lockRect.width()/15);
            QPointF menuPos = mapFromScene(lockRect.bottomLeft() + offset);
            menuPos = mapToGlobal(menuPos.toPoint());
            menu->popup(menuPos.toPoint());
            prevLockMenuOpened = menu;
        }
    }
}


/**
 * @brief NodeView::hardwareClusterChildrenViewMenuClosed
 * @param nodeItem
 */
void NodeView::hardwareClusterChildrenViewMenuClosed(NodeItem *nodeItem)
{
    QPointF viewPos = mapFromScene(nodeItem->geChildrenViewOptionMenuSceneRect().topLeft());
    QPointF globalPos = mapToGlobal(viewPos.toPoint());
    QRectF rect(globalPos, nodeItem->geChildrenViewOptionMenuSceneRect().size()*transform().m11());

    if (!rect.contains(QCursor::pos())) {
        prevLockMenuOpened = 0;
        //emit view_nodeItemLockMenuClosed(nodeItem);
    }
}

void NodeView::setState(NodeView::VIEW_STATE newState)
{

    if(viewState == VS_NONE){
        if(newState == VS_PAN){
            viewState = VS_PAN;
            setCursor(Qt::ClosedHandCursor);
        }else if(newState == VS_RUBBERBAND){
            viewState = VS_RUBBERBAND;
            setRubberBandMode(true);
            setCursor(Qt::CrossCursor);
        }else if(newState == VS_SELECTED){
            viewState = VS_SELECTED;
        }
    }else if(viewState == VS_RESIZING){
        if(newState == VS_NONE || newState == VS_SELECTED){
            viewState = newState;
            unsetCursor();
        }
    }else if(viewState == VS_SELECTED){
        if(newState == VS_NONE){
            viewState = VS_NONE;
        }else if(newState == VS_MOVING){
            viewState = VS_MOVING;
            triggerAction("Moving Selection");
            setCursor(Qt::SizeAllCursor);
        }else if(newState == VS_RESIZING){
            viewState = VS_RESIZING;
            triggerAction("Resizing Selection");
            setCursor(Qt::SizeAllCursor);
        }else if(newState == VS_PAN){
            viewState = VS_PAN;
        }else if(newState == VS_RUBBERBAND){
            viewState = VS_RUBBERBAND;
        }
    }else if(viewState == VS_MOVING){
        if(newState == VS_NONE || newState == VS_SELECTED){
            viewState = newState;
            unsetCursor();
        }
    }else if(viewState == VS_RUBBERBAND || viewState == VS_RUBBERBANDING){
        if(newState == VS_NONE || newState == VS_SELECTED){
            viewState = newState;
            setRubberBandMode(false);
            unsetCursor();
        }else{
            if(newState == VS_RUBBERBANDING || newState == VS_RUBBERBAND){
                viewState = newState;
            }
        }
    }else if(viewState == VS_PAN || viewState == VS_PANNING){
        if(newState == VS_NONE){
            viewState = VS_NONE;
        }else if(viewState != VS_PANNING && newState == VS_PANNING){
            setCursor(Qt::ClosedHandCursor);
            viewState = VS_PANNING;
            wasPanning = true;
        }else if(newState == VS_SELECTED){
            viewState = VS_SELECTED;
        }
        if(viewState == VS_NONE || viewState == VS_SELECTED){
            unsetCursor();
        }
    }

    //viewState = newState;

    if(viewState == VS_NONE){
        unsetCursor();
    }


}


void NodeView::_deleteFromIDs(QStringList IDs)
{
    if (IDs.count() > 0) {
        if(viewMutex.tryLock()){
            //Clear the Attribute Table Model
            setAttributeModel(0, true);
            emit view_Delete(IDs);
        }
    } else {
        view_displayNotification("Select entity(s)/connection(s) to delete.");
    }
}


/**
 * @brief NodeView::enableClipboardActions
 * @param IDs
 */
void NodeView::enableClipboardActions()
{
    if (controller) {
        emit view_updateMenuActionEnabled("cut", controller->canCut(selectedIDs));
        emit view_updateMenuActionEnabled("copy", controller->canCopy(selectedIDs));
        emit view_updateMenuActionEnabled("replicate", controller->canReplicate(selectedIDs));
        emit view_updateMenuActionEnabled("paste", controller->canPaste(selectedIDs));
        emit view_updateMenuActionEnabled("delete", controller->canDelete(selectedIDs));
        emit view_updateMenuActionEnabled("undo", controller->canUndo());
        emit view_updateMenuActionEnabled("redo", controller->canRedo());
        emit view_updateMenuActionEnabled("localdeployment", controller->canLocalDeploy());
    }

    emit view_updateMenuActionEnabled("noSelection", getSelectedNodeID() != "");
    emit view_updateMenuActionEnabled("nodesSelected", !getSelectedNodeIDs().isEmpty());
}


/**
 * @brief NodeView::showToolbar
 * This is called when there is a mouse right-click event.
 * It show/hides the context or window toolbar depending on where the right click happened.
 * @param position
 */
void NodeView::showToolbar(QPoint position)
{
    if(wasPanning){
        wasPanning = false;
        return;
    }

    // only show the toolbar if there is at least one node item selected
    if (selectedIDs.count() > 0) {

        NodeItem* selectedItem = getSelectedNodeItem();
        if (selectedItem && selectedItem->getNodeKind() == "Model") {
            return;
        }

        // this will update the toolbar buttons and menus
        updateToolbarSelectedItems(true);

        // use mouse click position when constructing node items from toolbar
        QPointF globalPos = mapToGlobal(position);
        toolbarPosition = mapToScene(position);

        bool toolbarPositionContained = false;

        foreach (QString ID, selectedIDs) {
            GraphMLItem* item = getGraphMLItemFromHash(ID);
            if (item->isNodeItem()) {
                if (item->sceneBoundingRect().contains(toolbarPosition)) {
                    toolbarPositionContained = true;
                    break;
                }
            } else if (item->isEdgeItem()) {
                if (eventFromEdgeItem) {
                    toolbarPositionContained = true;
                    eventFromEdgeItem = false;
                    break;
                }
            }
        }

        // this case happens when this is called from the window toolbar
        // show the toolbar positioned at the first selected item's center
        QList<NodeItem*> selectedNodeItems = getSelectedNodeItems();
        if (position.isNull() && selectedNodeItems.count() > 0) {
            QPointF itemScenePos = selectedNodeItems.at(0)->sceneBoundingRect().center();
            globalPos = mapFromScene(itemScenePos);
            globalPos = mapToGlobal(globalPos.toPoint());
            toolbarPosition = itemScenePos;
            toolbarPositionContained = true;
        }

        // only show the toolbar if the right-click happened inside one of the selected items
        if (toolbarPositionContained) {
            toolbar->move(globalPos.toPoint());
            toolbar->setVisible(true);
        }
    }
}


void NodeView::view_ConstructNodeGUI(Node *node)
{
    if(!node){
        qCritical() << "Node is Null.";
    }

    if(IS_SUB_VIEW){
        if(centralizedNodeItem && centralizedNodeItem->getNode()){
            Node* subView = centralizedNodeItem->getNode();
            if(!subView->isAncestorOf(node)){
                return;
            }
        }
    }



    Node* parentNode = node->getParentNode();

    QString nodeKind = node->getDataValue("kind");

    if(nonDrawnItemKinds.contains(nodeKind)){
        noGuiIDHash[node->getID()] = nodeKind;
        return;
    }

    NodeItem* parentNodeItem = 0;
    Node* modelParent = parentNode;
    while(modelParent){
        GraphMLItem* parentGUI = getGraphMLItemFromHash(modelParent->getID());
        parentNodeItem = getNodeItemFromGraphMLItem(parentGUI);
        if(parentNodeItem){
            break;
        }
        modelParent = modelParent->getParentNode();
    }

    if(!parentNodeItem && node->getDataValue("kind") != "Model"){
        qCritical() << "NodeView::view_ConstructNodeGUI() SUB_VIEW probably not meant to build this item as we don't have it's parent.";
        return;
    }

    //Expanded parent
    if(toolbarDockConstruction || importFromJenkins){
        if(parentNodeItem){
            if(!parentNodeItem->isExpanded()){
                parentNodeItem->setNodeExpanded(true);
            }
        }
    }


    //Check for Pasted data, which will have x,y set to -1
    //SHRINK SIZE.
    QString xPos = node->getDataValue("x");
    QString yPos = node->getDataValue("y");
    QString widthStr = node->getDataValue("width");
    QString heightStr = node->getDataValue("height");
    bool resetSize = xPos == yPos && xPos == "-1";

    //qreal multiplier = 1;
    if(resetSize){
        QPointF newPosition;
        if(parentNodeItem){
            QRectF itemRect;
            itemRect.setWidth(widthStr.toDouble());
            itemRect.setHeight(heightStr.toDouble());
            newPosition = parentNodeItem->getNextChildPos(itemRect);
        }
        node->getData("x")->setValue(QString::number(newPosition.x()));
        node->getData("y")->setValue(QString::number(newPosition.y()));
    }


    NodeItem* nodeItem = new NodeItem(node, parentNodeItem, currentAspects, IS_SUB_VIEW);


    nodeItem->setNodeView(this);
    storeGraphMLItemInHash(nodeItem);


    //Connect the Generic Functionality.
    connectGraphMLItemToController(nodeItem);



    // send/do necessary signals/updates when a node has been constructed
    nodeConstructed_signalUpdates(nodeItem);

    // if SELECT_ON_CONSTRUCTION, select node after construction and center on it
    // the node's label is automatically selected and editable
    if(toolbarDockConstruction && SELECT_ON_CONSTRUCTION){
        clearSelection(true, false);
        appendToSelection(nodeItem, false);
        nodeItem->setNewLabel();
        centerOnItem();
        toolbarDockConstruction = false;
    }

    // add the newly constructed node item to the scene
    if(!scene()->items().contains(nodeItem)){
        scene()->addItem(nodeItem);
    }
}


void NodeView::view_ConstructEdgeGUI(Edge *edge)
{

    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    if(IS_SUB_VIEW){
        if(centralizedNodeItem && centralizedNodeItem->getNode()){
            Node* subView = centralizedNodeItem->getNode();
            if(!(subView->isAncestorOf(src) && subView->isAncestorOf(dst))){
                return;
            }
        }
    }


    NodeItem* srcGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(src->getID()));
    NodeItem* dstGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(dst->getID()));

    if (edge->isDeploymentLink()) {
        updateDeployment = true;
    }
    //edgeConstructed_signalUpdates();

    if(srcGUI != 0 && dstGUI != 0){
        // send necessary signals when an edge has been constucted
        //We have valid GUI elements for both ends of this edge.
        bool constructEdge = true;

        if(edge->isAggregateLink() || edge->isDeploymentLink() || edge->isInstanceLink() || edge->isImplLink()){
            constructEdge = false;
        }

        if(edge->isAssemblyLink() || edge->isDelegateLink() || edge->isComponentLink()){
            constructEdge = true;
        }



        //Construct a new GUI Element for this edge.
        EdgeItem* nodeEdge = new EdgeItem(edge, srcGUI, dstGUI);

        nodeEdge->setNodeView(this);
        //Add it to the list of EdgeItems in the Model.
        storeGraphMLItemInHash(nodeEdge);

        connectGraphMLItemToController(nodeEdge);

        /*
        // show hidden hardware node before the new edge is added to scene
        // sort its parent so that the newly visible hardware node can be easily seen
        if (dstGUI->getGraphML()->getDataValue("kind") == "HardwareNode") {
            dstGUI->setHidden(false);
            if (dstGUI->getParentNodeItem()) {
                dstGUI->getParentNodeItem()->newSort();
            }
        }
        */

        if(!constructEdge){
            nodeEdge->setHidden(true);
        }
        if(!scene()->items().contains(nodeEdge)){
            //   //Add to model.
            scene()->addItem(nodeEdge);
        }





    }else{
        //Store non created edge ID
        noGuiIDHash[edge->getID()] = "Edge";
        if(!IS_SUB_VIEW){
            qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
        }
    }
}


/**
 * @brief NodeView::updateToolbarSelectedItems
 * @param showToolbar
 */
void NodeView::updateToolbarSelectedItems(bool showToolbar)
{
    if (!CONTROL_DOWN && !showToolbar) {
        return;
    }

    // only show the toolbar if there is at least one node item selected
    if (selectedIDs.count() > 0) {

        // get all the selected node items
        QList<NodeItem*> selectedNodeItems;
        foreach (NodeItem* item, getSelectedNodeItems()) {
            if (item->isPainted()) {
                selectedNodeItems.append(item);
            }
        }

        // get all the selected edge items
        QList<EdgeItem*> selectedEdgeItems;
        if (selectedNodeItems.count() != selectedIDs.count()) {
            foreach (EdgeItem* item, getEdgeItemsList()) {
                if (item->isVisible() && selectedIDs.contains(item->getID())) {
                    selectedEdgeItems.append(item);
                }
            }
        }

        toolbar->updateSelectedItems(selectedNodeItems, selectedEdgeItems);
    }
}

void NodeView::view_CenterGraphML(GraphML *graphML)
{
    //qCritical() << "Centering on: " << graphML->toString();
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    if(guiItem){
        centerItem(guiItem);
    }
}

void NodeView::view_LockCenteredGraphML(QString ID)
{
    if(ID == ""){
        return;
    }

    NodeItem* nodeItem = getNodeItemFromID(ID);
    if(nodeItem){
        centralizedItemID = ID;
        centralizedNodeItem = nodeItem;
        CENTRALIZED_ON_ITEM = true;
        connect(nodeItem, SIGNAL(nodeItemMoved()), this, SLOT(centralizedItemMoved()));
        appendToSelection(getGraphMLItemFromHash(ID));
        centerItem(ID);
        ensureAspect(ID);

    }
}

void NodeView::sort()
{
    sortSelection();
}


/**
 * @brief NodeView::constructNode
 * This is called when the user has either clicked on a dock item from
 * the parts dock or an action from the toolbar's addChildMenu.
 * @param nodeKind - kind of node to construct
 * @param sender - 0 = DockScrollArea, 1 = ToolbarWidget
 */
void NodeView::constructNode(QString nodeKind, int sender)
{
    if(viewMutex.tryLock()){
        NodeItem* selectedItem = getSelectedNodeItem();
        if (selectedItem) {
            toolbarDockConstruction = true;
            if (sender == 0) {
                // if from dock, place at next available position on grid
                view_ConstructNode(selectedItem->getID(), nodeKind, selectedItem->getNextChildPos());
            } else if (sender == 1) {
                // if from toolbar, place at closest grid point to the toolbar's position
                QPointF position = selectedItem->mapFromScene(toolbarPosition);
                QPointF newPosition = selectedItem->getClosestGridPoint(position);
                view_ConstructNode(selectedItem->getID(), nodeKind, newPosition);
            }
        }
    }
}


/**
 * @brief NodeView::constructEdge
 * This is called when the user has triggered to make a connection (edge)
 * using either the definitions/hardware dock or the toolbar's connectMenu.
 * @param src
 * @param dst
 * @param trigger
 */
void NodeView::constructEdge(QString srcID, QString dstID)
{
    if (viewMutex.tryLock()) {

        NodeItem* srcNode = getNodeItemFromID(srcID);
        NodeItem* dstNode = getNodeItemFromID(dstID);
        if (srcNode && dstNode) {
            view_displayNotification("Connected " + srcNode->getNodeLabel() +
                                     " to " + dstNode->getNodeLabel() + ".",
                                     notificationNumber, numberOfNotifications);
        }

        // reset notification seq and total number
        //numberOfNotifications = 1;
        //notificationNumber = 0;

        triggerAction("Dock/Toolbar: Constructing Edge");

        emit view_ConstructEdge(srcID, dstID);
    }
}


void NodeView::destructEdge(QString srcID, QString dstID, bool triggerAction)
{
    Q_UNUSED(triggerAction);

    if(viewMutex.tryLock()){
        NodeItem* srcNode = getNodeItemFromID(srcID);
        NodeItem* dstNode = getNodeItemFromID(dstID);
        if(srcNode && dstNode){
            view_displayNotification("Disconnected " + srcNode->getNodeLabel() +
                                     " from " + dstNode->getNodeLabel() + ".",
                                     notificationNumber, numberOfNotifications);
            // reset notification seq and total number
            //numberOfNotifications = 1;
            //notificationNumber = 0;
        }
        emit view_DestructEdge(srcID, dstID);
    }
}


/**
 * @brief NodeView::constructDestructEdges
 * @param srcIDs
 * @param dstID
 */
void NodeView::constructDestructEdges(QStringList srcIDs, QString dstID)
{
    if (viewMutex.tryLock()) {

        if (!srcIDs.isEmpty()) {

            NodeItem* dstNode = getNodeItemFromID(dstID);
            if (dstNode) {
                /*
                view_displayNotification("Connected selection to " +
                                         dstNode->getNodeLabel() + ".",
                                         notificationNumber, numberOfNotifications);
                                         */
            }

            triggerAction("Dock: Destructing/Constructing Multiple Edges");
            emit view_constructDestructEdges(srcIDs, dstID);
        }

    }
}


void NodeView::deleteFromIDs(QStringList IDs)
{
    _deleteFromIDs(IDs);
}

/**
 * @brief NodeView::constructConnectedNode
 * @param parentID - selected node's parent node
 * @param dstID - selected node
 * @param kind - node kind to construct
 * @param sender - 0 = DockScrollArea, 1 = ToolbarWidget
 */
void NodeView::constructConnectedNode(QString parentID, QString dstID, QString kind, int sender)
{
    if(viewMutex.tryLock()){
        NodeItem *nodeItem = getNodeItemFromID(parentID);
        if(nodeItem){
            toolbarDockConstruction = true;
            QPointF position;

            if (sender == 0){
                position = nodeItem->getNextChildPos();
            } else if (sender == 1) {
                // if from toolbar, place at closest grid point to the toolbar's position
                QPointF closestGridPos = nodeItem->getClosestGridPoint(nodeItem->mapFromScene(toolbarPosition));
                position = closestGridPos;
            }
            emit view_ConstructConnectedNode(parentID, dstID, kind, position);
        }
    }
}


/**
 * @brief NodeView::showConnectedNodes
 * This method shows, selects and centers all of the nodes
 * that are connected to the currently selected node.
 */
void NodeView::showConnectedNodes()
{
    Node* node = getSelectedNode();

    if (node) {

        QList<NodeItem*> connectedNodeItems;

        // store the outer edges of the selected node
        foreach (Edge* edge, node->getEdges(0)) {
            if (!node->isAncestorOf(edge->getSource())) {
                connectedNodeItems.append(getNodeItemFromNode(edge->getSource()));
            }
            if (!node->isAncestorOf(edge->getDestination())) {
                connectedNodeItems.append(getNodeItemFromNode(edge->getDestination()));
            }
        }

        if (connectedNodeItems.count() > 0) {
            foreach (NodeItem* item, connectedNodeItems) {
                // make sure the aspect(s) that the nodeItem belongs to is turned on
                foreach (QString aspect, item->getAspects()) {
                    addAspect(aspect);
                }
                // add connected nodes to selection
                appendToSelection(item);
            }
            // add the selected node to the list of items to center
            connectedNodeItems.append(getNodeItemFromNode(node));
            // fit the connected nodes' rectangle to the screen
            fitToScreen(connectedNodeItems, 1.35);
        }
    }
}






void NodeView::setGraphMLItemSelected(GraphMLItem *item, bool setSelected)
{
    QString itemID = item->getGraphML()->getID();

    if(setSelected){
        if(!selectedIDs.contains(itemID)){
            int nodeItemSize = 0;
            if(item->isNodeItem()){
                nodeItemSize = ((NodeItem*)item)->getNode()->getTreeIndex().size();
            }
            //Find spot for selectedID;
            int position = selectedIDs.count();
            for(int i = 0;i < selectedIDs.count();i++){
                NodeItem* node = getNodeItemFromID(selectedIDs[i]);
                if(node){
                    int currentPos = node->getNode()->getTreeIndex().size();
                    if(nodeItemSize > currentPos){
                        position = i;
                        break;

                    }
                }
            }
            selectedIDs.insert(position, itemID);
            item->setSelected(true);
            if(item->isNodeItem()){
                nodeSelected_signalUpdates();
            }

        }
    }else{
        if(selectedIDs.contains(itemID)){
            setAttributeModel(0);
            selectedIDs.removeAll(itemID);


            item->setSelected(false);
            if(item->isNodeItem()){
                nodeSelected_signalUpdates();
            }
        }
    }

    if(selectedIDs.count() == 1){
        GraphMLItem* item = getGraphMLItemFromHash(selectedIDs.last());
        if(item){
            setAttributeModel(item);
            return;
        }
    }
    setAttributeModel();
}


NewController *NodeView::getController()
{
    return this->controller;
}


/**
 * @brief NodeView::getFiles
 * TODO: Get Definitions containers from the controller and use them to get
 * the Files, Components, etc instead of recursing throught the whole model.
 * @return - list of IDL files
 */
QList<Node*> NodeView::getFiles()
{
    QList<Node*> returnList;
    Model* model = controller->getModel();
    if (model) {
        returnList = model->getChildrenOfKind("IDL");
    }
    return returnList;
}


/**
 * @brief NodeView::getComponents
 * @return - list of Component definitions
 */
QList<Node*> NodeView::getComponents()
{
    QList<Node*> returnList;
    Model* model = controller->getModel();
    if (model) {
        returnList = model->getChildrenOfKind("Component");
    }
    return returnList;
}


/**
 * @brief NodeView::getBlackBoxes
 * @return - list of BlackBox definitions
 */
QList<Node*> NodeView::getBlackBoxes()
{
    QList<Node*> returnList;
    Model* model = controller->getModel();
    if (model) {
        returnList = model->getChildrenOfKind("BlackBox");
    }
    return returnList;
}


/**
 * @brief NodeView::getConstructableNodeKinds
 * This method gets the constuctable node kinds list from the controller.
 * @return
 */
QStringList NodeView::getGUIConstructableNodeKinds()
{
    if(controller){
        return controller->getGUIConstructableNodeKinds();
    }
    return QStringList();
}

QStringList NodeView::getAllNodeKinds()
{
    if(controller){
        return controller->getAllNodeKinds();
    }
    return QStringList();
}



void NodeView::appendToSelection(Node *node)
{
    appendToSelection(getGraphMLItemFromGraphML(node));
}


/**
 * @brief NodeView::updateViewCenterPoint
 * This updates the previous and current view center point.
 * This needs to be called every time the view is centered on something.
 */
void NodeView::updateViewCenterPoint()
{
    prevCenterPoint = centerPoint;
    centerPoint = getVisibleRect().center();
}


/**
 * @brief NodeView::getPreviousViewCenterPoint
 * @return - viewport's previous center point
 */
QPointF NodeView::getPreviousViewCenterPoint()
{
    return prevCenterPoint;
}

bool NodeView::managementComponentsShown()
{
    return managementComponentVisible;
}


/**
 * @brief NodeView::recenterView
 * This is called every time the MEDEA window is resized.
 * It recenters the view on its previous center point before the window was resized.
 */
void NodeView::recenterView()
{
    centerViewOn(prevCenterPoint);
}


/**
 * @brief NodeView::getAllAspects
 * @return - a list of all the view aspects
 */
QStringList NodeView::getAllAspects()
{
    return allAspects;
}


/**
 * @brief NodeView::viewDeploymentAspect
 * This makes sure that the view aspects needed for Deployment are turned on.
 */
void NodeView::viewDeploymentAspect()
{
    // only show a notification if there has been a change in view aspects
    if (!currentAspects.contains("Assembly") || !currentAspects.contains("Hardware")) {
        view_displayNotification("Turned on Deployment view aspects.",  notificationNumber, numberOfNotifications);
    } else if (numberOfNotifications > 1) {
        view_displayNotification("",  notificationNumber, numberOfNotifications);
    }
    addAspect("Assembly");
    addAspect("Hardware");
}

QImage NodeView::getImage(QString imageName)
{
    if(imageLookup.contains(imageName)){
        return imageLookup[imageName];
    }else{
        QImage image(":/Resources/Icons/" + imageName + ".png");
        imageLookup[imageName] = image;
        return image;
    }

}

NodeItem *NodeView::getImplementation(QString ID)
{
    NodeItem* impl = 0;
    if(controller){
        QString implID = controller->getImplementation(ID);
        impl = getNodeItemFromID(implID);
    }
    return impl;
}

QList<NodeItem *> NodeView::getInstances(QString ID)
{
    QList<NodeItem*> instances;
    if(controller){
        foreach(QString instanceID, controller->getInstances(ID)){
            NodeItem* instance = getNodeItemFromID(instanceID);
            if(instance){
                instances.append(instance);
            }
        }
    }
    return instances;
}

NodeItem *NodeView::getDefinition(QString ID)
{
    NodeItem* def = 0;
    if(controller){
        QString defID = controller->getDefinition(ID);
        def = getNodeItemFromID(defID);
    }
    return def;
}

QString NodeView::getDefinitionID(QString ID)
{
    if(controller){
        return controller->getDefinition(ID);
    }
    return "";
}

NodeItem *NodeView::getAggregate(QString ID)
{
    NodeItem* aggr = 0;
    if(controller){
        QString aggID = controller->getAggregate(ID);
        aggr = getNodeItemFromID(aggID);
    }
    return aggr;
}

NodeItem *NodeView::getDeployedNode(QString ID)
{
    NodeItem* hardwareItem = 0;
    if(controller){
        QString deplyID = controller->getDeployedHardwareID(ID);
        hardwareItem = getNodeItemFromID(deplyID);
    }
    return hardwareItem;
}


void NodeView::connectGraphMLItemToController(GraphMLItem *GUIItem)
{
    if(GUIItem){


        NodeItem* nodeItem = 0;
        if(GUIItem->isNodeItem()){
            nodeItem = (NodeItem*) GUIItem;
        }

        if (GUIItem->isEdgeItem()) {
            EdgeItem* edgeItem = (EdgeItem*) GUIItem;
            connect(edgeItem, SIGNAL(edgeItem_eventFromItem()), this, SLOT(setEventFromEdgeItem()));
        }

        if(GUIItem){
            connect(GUIItem, SIGNAL(GraphMLItem_ClearSelection(bool)), this, SLOT(clearSelection(bool)));
            connect(GUIItem, SIGNAL(GraphMLItem_AppendSelected(GraphMLItem*)), this, SLOT(appendToSelection(GraphMLItem*)));
            connect(GUIItem, SIGNAL(GraphMLItem_RemoveSelected(GraphMLItem*)), this, SLOT(removeFromSelection(GraphMLItem*)));
            connect(GUIItem, SIGNAL(GraphMLItem_PositionSizeChanged(GraphMLItem*,bool)), this, SLOT(keepSelectionFullyVisible(GraphMLItem*,bool)));

            //connect(GUIItem, SIGNAL(GraphMLItem_MovedOutOfScene(GraphMLItem*)), this, SLOT(fitInSceneRect(GraphMLItem*)));
        }
        if(nodeItem){
            connect(this, SIGNAL(view_AspectsChanged(QStringList)), nodeItem, SLOT(aspectsChanged(QStringList)));
        }

        if(!IS_SUB_VIEW){
            connect(GUIItem, SIGNAL(GraphMLItem_SetCentered(GraphMLItem*)), this, SLOT(centerItem(GraphMLItem*)));
            connect(GUIItem, SIGNAL(GraphMLItem_TriggerAction(QString)),  this, SLOT(triggerAction(QString)));

            connect(GUIItem, SIGNAL(GraphMLItem_SetGraphMLData(QString,QString,QString)), this, SIGNAL(view_SetGraphMLData(QString,QString,QString)));
            connect(GUIItem, SIGNAL(GraphMLItem_ConstructGraphMLData(GraphML*,QString)), this, SIGNAL(view_ConstructGraphMLData(GraphML*,QString)));
            connect(GUIItem, SIGNAL(GraphMLItem_DestructGraphMLData(GraphML*,QString)), this, SIGNAL(view_DestructGraphMLData(GraphML*,QString)));

            connect(GUIItem, SIGNAL(GraphMLItem_CenterAspects()), this, SLOT(fitToScreen()));

            if(nodeItem){
                if(nodeItem->getNodeKind() == "Model"){
                    connect(nodeItem, SIGNAL(model_PositionChanged()), this, SIGNAL(view_ModelSizeChanged()));
                }

                connect(nodeItem, SIGNAL(NodeItem_MoveSelection(QPointF)), this, SLOT(moveSelection(QPointF)));
                connect(nodeItem, SIGNAL(NodeItem_ResizeSelection(QString, QSizeF)), this, SLOT(resizeSelection(QString, QSizeF)));
                connect(nodeItem, SIGNAL(NodeItem_SortModel()), this, SLOT(sortModel()));
                connect(nodeItem, SIGNAL(NodeItem_MoveFinished()), this, SLOT(moveFinished()));
                connect(nodeItem, SIGNAL(NodeItem_ResizeFinished(QString)), this, SLOT(resizeFinished(QString)));
                connect(this, SIGNAL(view_toggleGridLines(bool)), nodeItem, SLOT(toggleGridLines(bool)));
                connect(nodeItem, SIGNAL(Nodeitem_HasFocus(bool)), this, SLOT(editableItemHasFocus(bool)));

                connect(nodeItem, SIGNAL(NodeItem_showLockMenu(NodeItem*)), this, SLOT(showHardwareClusterChildrenViewMenu(NodeItem*)));
                connect(nodeItem, SIGNAL(NodeItem_lockMenuClosed(NodeItem*)), this, SLOT(hardwareClusterChildrenViewMenuClosed(NodeItem*)));
                connect(this, SIGNAL(view_edgeConstructed()), nodeItem, SLOT(updateChildrenViewMode()));
                connect(nodeItem, SIGNAL(nodeItem_menuClicked(int)), toolbar, SLOT(hardwareClusterMenuClicked(int)));

            }
        }
    }
}



NodeItem *NodeView::getNodeItemFromGraphMLItem(GraphMLItem *item)
{
    if(item && item->isNodeItem()){
        return (NodeItem*) item;
    }
    return 0;
}

EdgeItem *NodeView::getEdgeItemFromGraphMLItem(GraphMLItem *item)
{
    if(item && item->isEdgeItem()){
        return (EdgeItem*) item;
    }
    return 0;
}


void NodeView::storeGraphMLItemInHash(GraphMLItem *item)
{
    GraphML* graphML = item->getGraphML();
    if(graphML){
        QString ID = graphML->getID();
        if(guiItems.contains(ID)){
            qCritical() << "Hash already contains GraphMLItem with ID: " << ID;
        }else{
            guiItems[ID] = item;
        }
    }

}

bool NodeView::removeGraphMLItemFromHash(QString ID)
{
    if(guiItems.contains(ID)){
        GraphMLItem* item = guiItems[ID];
        guiItems.remove(ID);

        if(selectedIDs.contains(ID)){
            selectedIDs.removeAll(ID);
        }


        if(ID == currentTableID){
            setAttributeModel(0);
        }


        if(item){
            if(item->isNodeItem()){
                NodeItem* nodeItem = (NodeItem*)item;
                if(nodeItem->getParentNodeItem()){
                    emit view_nodeDeleted(nodeItem->getID(), nodeItem->getParentNodeItem()->getID());
                }
            }else if (item->isEdgeItem()){
                EdgeItem* edgeItem = (EdgeItem*)item;
                updateDeployment = true;
                if(edgeItem->getSource() && edgeItem->getDestination()){
                    emit view_edgeDeleted(edgeItem->getSource()->getID(), edgeItem->getDestination()->getID());
                }
            }




            item->detach();
            //FIXED BUGS.
            //scene()->removeItem(item);
            //item->deleteLater();
            delete item;

        }
        if(IS_SUB_VIEW){
            if(CENTRALIZED_ON_ITEM && centralizedItemID == ID){
                //Delete the nodeView
                deleteLater();
            }
        }
        return true;
    }
    return false;
}


/**
 * @brief NodeView::nodeConstructed_signalUpdates
 * This is called whenever a node is constructed.
 * It sends signals to update whatever needs updating.
 * @param nodeItem
 */
void NodeView::nodeConstructed_signalUpdates(NodeItem* nodeItem)
{
    // update the docks and the toolbar/menu goTo functions
    //updateActionsEnabled();
    emit view_nodeConstructed(nodeItem);

    // send specific current view states to the newly constaructed node item
    //view_AspectsChanged(currentAspects);
    nodeItem->aspectsChanged(currentAspects);
    emit view_toggleGridLines(GRID_LINES_ON);

    // snap node item to its parent's grid
    //nodeItem->snapToGrid();

    if (nodeItem->getNodeKind().startsWith("Hardware")) {

        // this will update the HardwareClusters' children view mode
        // and the HardwareNodes' initial visibility
        updateDeployment = true;

    } else if (nodeItem->getNodeKind() == "ManagementComponent") {

        bool show = managementComponentsShown();
        if (isSubView() && parentNodeView) {
            show = parentNodeView->managementComponentsShown();
        }
        nodeItem->setHidden(!show);

    } else if (nodeItem->getNodeKind() == "AggregateInstance") {

        // hide all AggregateInstances except for in OutEventPortImpls
        NodeItem* parentItem = nodeItem->getParentNodeItem();
        if (parentItem && (parentItem->getNodeKind() != "AggregateInstance"
                           && parentItem->getNodeKind() != "Aggregate"
                           && parentItem->getNodeKind() != "OutEventPortImpl"))
        {
            nodeItem->setHidden(true);
        }

    }

    /*
    //Hide HardwareNodes.
    if (nodeItem->getNodeKind() == "HardwareNode") {
        nodeItem->setHidden(false);
    }
    */

    // if currently pasting, select pasted item
    if (pasting || importFromJenkins) {
        appendToSelection(nodeItem);
    }
}




/**
 * @brief NodeView::nodeSelected_signalUpdates
 * This gets called whenever a node has been selected.
 * It sends signals to update whatever needs updating.
 * @param node
 */
void NodeView::nodeSelected_signalUpdates()
{
    updateActionsEnabled();

    // update the highlighted deployment nodes.
    if (hardwareDockOpen) {
        highlightDeployment();
    }
    emit view_nodeSelected();
}


/**
 * @brief NodeView::edgeConstructed_signalUpdates
 * This gets called whenever an edge is constructed.
 * It sends signals to update whatever needs updating.
 * @param edge
 */
void NodeView::edgeConstructed_signalUpdates()
{
    // update the highlighted deployment nodes.
    if(hardwareDockOpen){
        highlightDeployment();
    }
    // update the docks
    emit view_edgeConstructed();
}


/**
 * @brief NodeView::edgeDestructed_signalUpdates
 * This gets called whenever an edge is destructed.
 * It sends signals to update whatever needs updating.
 * @param edge
 * @param ID
 */
void NodeView::edgeDestructed_signalUpdates(Edge* edge, QString ID)
{
    // update the toolbar/menu goTo functions
    //emit view_edgeDestructed();

    // check if destructed edge's destination is a HardwareNode
    NodeItem* destination = getNodeItemFromNode(edge->getDestination());
    if (destination && destination->getNodeKind() == "HardwareNode") {
        Node* node = destination->getNode();
        bool hasEdges = false;
        // check if it is connected to anything else
        foreach (Edge* edge, node->getEdges()) {
            if (edge->isDeploymentLink() && edge->getID() != ID) {
                hasEdges = true;
                break;
            }
        }
        if (!hasEdges) {
            // if it's not, hide the HarwareNode node item
            destination->setHidden(true);
        }
    }
}


/**
 * @brief NodeView::updateActionsEnabled
 * This method updates the enabled state of the MEDEA window's menu actions
 * and the toolbar's tool buttons for the goToDefinition/goToImplementation
 * functions based on the currently selected node.
 * @param selectedNode
 */
void NodeView::updateActionsEnabled()
{
    QString ID = getSelectedNodeID();

    QString defnID;
    QString implID;
    bool canExport = false;
    bool canImport = false;

    if (controller) {
        defnID = controller->getDefinition(ID);
        implID = controller->getImplementation(ID);
        canExport = controller->canExportSnippet(selectedIDs);
        canImport = controller->canImportSnippet(selectedIDs);

        emit view_updateMenuActionEnabled("undo", controller->canUndo());
        emit view_updateMenuActionEnabled("redo", controller->canRedo());
    }


    // update menu actions
    emit view_updateMenuActionEnabled("definition", defnID != "");
    emit view_updateMenuActionEnabled("implementation", implID != "");
    emit view_updateMenuActionEnabled("exportSnippet", canExport);
    emit view_updateMenuActionEnabled("importSnippet", canImport);

    // update toolbar buttons
    toolbar->showDefinitionButton(defnID);
    toolbar->showImplementationButton(implID);
    toolbar->showSnippetButton("export", canExport);
    toolbar->showSnippetButton("import", canImport);

    // update the toolbar's separators in case any of them need to be hidden
    toolbar->updateSeparators();
}

/*

Node* NodeView::hasDefinition(Node* node)
{
    Node* original = node;
    while (node->getDefinition()) {
        node = node->getDefinition();
    }
    if (node != original) {
        return node;
    }
    return 0;
}



Node* NodeView::hasImplementation(Node* node)
{
    Node* original = node;
    node = hasDefinition(node);
    if (!node) {
        node = original;
    }
    if (node->getImplementations().size() > 0) {
        Node* impl =node->getImplementations().at(0);
        if (impl != original) {
            return impl;
        }
    }
    return 0;
}
*/


bool NodeView::isItemsAncestorSelected(GraphMLItem *selectedItem)
{
    GraphML* selectedModelItem = selectedItem->getGraphML();
    if(selectedModelItem->isEdge()){
        return false;
    }
    Node* selectedModelNode = (Node*) selectedModelItem;
    QString ID = selectedModelNode->getID();

    foreach(QString ID, selectedIDs){
        GraphMLItem* item = getGraphMLItemFromHash(ID);
        GraphML* modelItem = item->getGraphML();

        if(modelItem && modelItem->isNode()){
            Node* modelNode = (Node*)modelItem;
            if(modelNode->isAncestorOf(selectedModelNode)){
                return true;
            }
        }
    }
    return false;
}

void NodeView::unsetItemsDescendants(GraphMLItem *selectedItem)
{
    GraphML* selectedModelItem = selectedItem->getGraphML();
    if(selectedModelItem->isEdge()){
        return;
    }

    Node* selectedModelNode = (Node*) selectedModelItem;


    QStringList currentlySelectedIDs;
    currentlySelectedIDs.append(selectedIDs);

    while(!currentlySelectedIDs.isEmpty()){
        QString ID = currentlySelectedIDs.takeFirst();
        GraphMLItem* item = getGraphMLItemFromHash(ID);
        GraphML* modelItem = item->getGraphML();

        bool remove = false;
        if(modelItem && modelItem->isNode()){
            Node* modelNode = (Node*)modelItem;
            remove = selectedModelNode->isAncestorOf(modelNode);
        }else if(modelItem && modelItem->isEdge()){
            Edge* modelEdge = (Edge*)modelItem;
            Node* src = modelEdge->getSource();
            Node* dst = modelEdge->getDestination();
            remove = selectedModelNode == src || selectedModelNode == dst;
        }
        if(remove){
            selectedIDs.removeAll(ID);
            item->setSelected(false);
        }
    }
}

NodeItem *NodeView::getNodeItemFromNode(Node *node)
{
    return getNodeItemFromGraphMLItem(getGraphMLItemFromGraphML(node));
}

NodeItem *NodeView::getNodeItemFromID(QString ID)
{
    NodeItem* node = 0;
    GraphMLItem* g = getGraphMLItemFromHash(ID);
    if(g && g->isNodeItem()){
        node = (NodeItem*) g;
    }
    return node;
}

GraphMLItem *NodeView::getGraphMLItemFromHash(QString ID)
{
    if(guiItems.contains(ID)){
        return guiItems[ID];
    }else{
        //if(!IS_SUB_VIEW){
        //qCritical() << "Cannot find GraphMLItem from Lookup Hash. ID: " << ID;
        //}
    }
    return 0;
}

GraphMLItem *NodeView::getGraphMLItemFromScreenPos(QPoint pos)
{
    QPointF scenePos = mapToScene(pos);
    QGraphicsItem* item = scene()->itemAt(scenePos, QTransform());
    return dynamic_cast<GraphMLItem*>(item);
}



GraphMLItem *NodeView::getGraphMLItemFromGraphML(GraphML *item)
{
    if(item){
        return getGraphMLItemFromHash(item->getID());
    }
    return 0;
}


/**
 * @brief NodeView::mouseReleaseEvent
 * @param event
 */
void NodeView::mouseReleaseEvent(QMouseEvent *event)
{
    if(MINIMAP_EVENT){
        QGraphicsView::mouseReleaseEvent(event);
        return;
    }

    if(viewState == VS_RUBBERBANDING){
        if(event->button() == Qt::LeftButton){
            QPoint screenOrigin = rubberBand->pos();
            QPoint screenFinish = screenOrigin + QPoint(rubberBand->width(), rubberBand->height());
            QPointF sceneOrigin = mapToScene(screenOrigin);
            QPointF sceneFinish = mapToScene(screenFinish);
            selectedInRubberBand(sceneOrigin, sceneFinish);
            rubberBand->setVisible(false);
            setState(VS_RUBBERBAND);
        }
        return;
    }else if(viewState == VS_NONE){
        if(event->button() == Qt::MiddleButton){
            GraphMLItem* item = getGraphMLItemFromScreenPos(event->pos());

            if(!item){
                //If we don't have an item under the mouse. Fit screen to view.
                fitToScreen();
            }
        }
        return;
    }else if(viewState == VS_PAN || viewState == VS_PANNING){
        if(selectedIDs.length() > 0){
            setState(VS_SELECTED);
        }else{
            setState(VS_NONE);
        }
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}


/**
 * @brief NodeView::mouseMoveEvent
 * @param event
 */
void NodeView::mouseMoveEvent(QMouseEvent *event)
{
    qCritical() << getViewState();
    if(MINIMAP_EVENT){
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    if(viewState == VS_RUBBERBANDING){
        rubberBand->setGeometry(QRect(rubberBandOrigin, event->pos()).normalized());
        return;
    }else if(viewState == VS_PAN || viewState == VS_PANNING){
        if(viewState == VS_PAN){
            setState(VS_PANNING);
        }

        QPointF currentScenePos = mapToScene(event->pos());
        QPointF delta = currentScenePos - panningSceneOrigin;
        panningSceneOrigin = currentScenePos;

        adjustModelPosition(delta);
        return;
    }else{
        QGraphicsView::mouseMoveEvent(event);
    }
}


/**
 * @brief NodeView::mousePressEvent
 * NOTE: If there is a selected item, right clicking on one of its
 * child doesn't select the child and update the toolbar.
 * @param event
 */
void NodeView::mousePressEvent(QMouseEvent *event)
{
    if(toolbarJustClosed){
        toolbarJustClosed = false;
        return;
    }

    // TODO: Need to catch the case where the menu is closed
    // when MEDEA window steals the focus
    // need this in case there is an opened lock menu
    if (prevLockMenuOpened && prevLockMenuOpened->isVisible()) {
        prevLockMenuOpened->close();
    }


    if(viewState == VS_NONE || viewState == VS_SELECTED){
        GraphMLItem* item = getGraphMLItemFromScreenPos(event->pos());
        if(!item && (event->button() == Qt::LeftButton)){
            // clear the selection and disable dock buttons
            clearSelection();
        }
        if(event->button() == Qt::RightButton){
            setState(VS_PAN);
            panningSceneOrigin = mapToScene(event->pos());
            return;
        }
    }else if(viewState == VS_RUBBERBAND){
        if(event->button() == Qt::LeftButton){
            setState(VS_RUBBERBANDING);
            rubberBandOrigin = event->pos();
            //Move rubberband to the position on the screen.
            rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
            rubberBand->setVisible(true);
        }
        return;
    }


    QGraphicsView::mousePressEvent(event);
}


/**
 * @brief NodeView::wheelEvent
 * This method zooms in/out of the view upto the MIN/MAX ZOOM values.
 * @param event
 */
void NodeView::wheelEvent(QWheelEvent *event)
{
    scrollEvent(event->delta());
}


/**
 * @brief NodeView::mouseDoubleClickEvent
 * @param event
 */
void NodeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    // need this in case there is an opened lock menu
    if (prevLockMenuOpened && prevLockMenuOpened->isVisible()) {
        prevLockMenuOpened->close();
    }

    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* itemUnderMouse = scene()->itemAt(scenePos, QTransform());

    // added this to center aspects when double-clicking on the view
    if(!itemUnderMouse && event->button() == Qt::LeftButton){
        fitToScreen();
    }

    QGraphicsView::mouseDoubleClickEvent(event);
}


/**
 * @brief NodeView::keyPressEvent
 * @param event
 */
void NodeView::keyPressEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    //Only allow transition from none state.
    if((viewState == VS_NONE || viewState == VS_SELECTED) && (CONTROL && SHIFT)){
        setState(VS_RUBBERBAND);
    }

    if(hasFocus()){
        if(CONTROL){
           if(event->key() == Qt::Key_A){
               if (!editingNodeItemLabel) {
                   selectAll();
               }
           }else if(event->key() == Qt::Key_D){
               replicate();
           }
        }

        if (event->key() == Qt::Key_Escape){
            setState(VS_NONE);
            clearSelection();
        }
    }

    QGraphicsView::keyPressEvent(event);
}


/**
 * @brief NodeView::keyReleaseEvent
 * @param event
 */
void NodeView::keyReleaseEvent(QKeyEvent *event)
{
    bool allowedFocusWidget = allowedFocus(focusWidget());


    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    bool rubberbandMode = CONTROL && SHIFT;

    //Only allow transition from none state.
    if(!rubberbandMode && (viewState == VS_RUBBERBAND || viewState == VS_RUBBERBANDING)){
        //Only allow transition from none state.
        if(selectedIDs.length() > 0){
            setState(VS_SELECTED);
        }else{
            setState(VS_NONE);
        }
    }


    if(allowedFocusWidget){
        if(event->key() == Qt::Key_Delete){

            deleteSelection();
        }
    }

    QGraphicsView::keyReleaseEvent(event);

}


/**
 * @brief NodeView::alignSelectionHorizontally
 * This is called when the alignHorizontal button on the toolbar is clicked.
 * It aligns the selected items horizontally.
 */
void NodeView::alignSelectionHorizontally()
{
    if (selectedIDs.count() > 0) {
        alignSelectionOnGrid(HORIZONTAL);
    } else {
        view_displayNotification("No selected entities to align.");
    }
}


/**
 * @brief NodeView::alignSelectionVertically
 * This is called when the alignVertical button on the toolbar is clicked.
 * It aligns the selected items vertically.
 */
void NodeView::alignSelectionVertically()
{
    if (selectedIDs.count() > 0) {
        alignSelectionOnGrid(VERTICAL);
    } else {
        view_displayNotification("No selected entities to align.");
    }
}


void NodeView::alignSelectionOnGrid(NodeView::ALIGN alignment)
{
    int itemCount=0;
    qreal averageY=0;
    qreal averageX=0;

    QGraphicsItem* sharedParent = 0;
    if(!(alignment == VERTICAL || alignment == HORIZONTAL)){
        return;
    }



    foreach(QString ID, selectedIDs){
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
        if(graphMLItem && graphMLItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            if(!sharedParent){
                sharedParent = graphMLItem->parentItem();
            }else if(sharedParent != graphMLItem->parentItem()){
                //showDialogMessage(WARNING, "Selection Issue", "Cannot Align Selection which aren't contained by the same Parent.", graphMLItem->getGraphML(), true);
                view_displayNotification("Cannot align entities which aren't contained by the same parent.");
                return;
            }
            averageX += nodeItem->centerPos().x();
            averageY += nodeItem->centerPos().y();
            itemCount++;
        }
    }
    triggerAction("Aligning Selection");
    averageX /= itemCount;
    averageY /= itemCount;

    QPointF center;
    if(sharedParent){
        //Find closest Grid Line
        NodeItem* parent = (NodeItem*)sharedParent;
        center = parent->getClosestGridPoint(QPointF(averageX, averageY));
    }

    foreach(QString ID, selectedIDs){
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
        if(graphMLItem && graphMLItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            QPointF pos = nodeItem->centerPos();

            if(alignment == VERTICAL){
                pos.setX(center.x());
            }
            if(alignment == HORIZONTAL){
                pos.setY(center.y());
            }
            pos = nodeItem->getParentNodeItem()->getClosestGridPoint(pos);

            nodeItem->setCenterPos(pos);
            nodeItem->updateModelPosition();
        }
    }
}



/**
 * @brief NodeView::snapSelectionToGrid
 * This is called from either the menu or toolbar.
 * This snaps each selected items to their closest grid position.
 */
void NodeView::snapSelectionToGrid()
{
    foreach (QString ID, selectedIDs) {
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
        if (graphMLItem && graphMLItem->isNodeItem()) {
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            nodeItem->snapToGrid();
        }
    }
}


/**
 * @brief NodeView::snapChildrenToGrid
 * This is called from either the menu or toolbar.
 * This snaps the selected item's children to their closest grid position.
 */
void NodeView::snapChildrenToGrid()
{
    NodeItem* currentSelected= getSelectedNodeItem();
    if (currentSelected) {
        currentSelected->snapChildrenToGrid();
    }
}


void NodeView::setDefaultAspects()
{
    currentAspects.clear();
    setAspects(defaultAspects);
}

void NodeView::setEnabled(bool enabled)
{
    //HIDE STUFF
    QGraphicsView::setEnabled(enabled);


    //if(enabled){
    //    viewMutex.unlock();
    //}
}


void NodeView::showMessage(MESSAGE_TYPE type, QString title, QString message, QString ID, bool centralizeItem)
{
    GraphMLItem* item = getGraphMLItemFromHash(ID);
    if(item && centralizeItem){
        centerItem(item);
    }
    if(message != ""){
        if(type == CRITICAL){
            QMessageBox::critical(this, "Error: " + title, message, QMessageBox::Ok);
        }else if(type == WARNING){
            //QMessageBox::warning(this, "Warning: " + title, message, QMessageBox::Ok);
            emit view_displayNotification(message);
        }else{
            //QMessageBox::information(this, "Message: " + title, message, QMessageBox::Ok);
            if (type == MODEL) {
                QMessageBox::information(this, "Message: " + title, message, QMessageBox::Ok);
            } else {
                emit view_displayNotification(message);
            }
        }
    }
}


/**
 * @brief NodeView::duplicate
 */
void NodeView::replicate()
{
    if (selectedIDs.count() > 0) {
        if(viewMutex.tryLock()){
            pasting = true;
            //Clear before pasting
            QStringList duplicateList = selectedIDs;
            clearSelection();
            emit view_Replicate(duplicateList);
        }
    } else {
        view_displayNotification("Select entity(s) to replicate.");
    }
}


/**
 * @brief NodeView::copy
 */
void NodeView::copy()
{
    if (selectedIDs.count() > 0) {
        if(viewMutex.tryLock()){
            emit view_Copy(selectedIDs);
        }
    } else {
        view_displayNotification("Select entity(s) to copy.");
    }
}


/**
 * @brief NodeView::cut
 */
void NodeView::cut()
{
    //If we have got something selected.
    if (selectedIDs.count() > 0) {
        //Try and Lock the Mutex before the operation.
        if(viewMutex.tryLock()){
            //Clear the Attribute Table Model
            setAttributeModel(0, true);
            emit view_Cut(selectedIDs);
        }
    } else {
        view_displayNotification("Select entity(s) to cut.");
    }
}


/**
 * @brief NodeView::paste
 * @param xmlData
 */
void NodeView::paste(QString xmlData)
{
    QString ID = getSelectedNodeID();

    if (ID == "") {
        //view_displayNotification("Select entity to paste into.");
        return;
    }

    if(viewMutex.tryLock()){
        //Pasting gets cleared in actionFinished()
        pasting = true;
        //Clear before pasting
        clearSelection();
        //Tell the Controller to paste.
        emit view_Paste(ID, xmlData);
    }
}


void NodeView::selectAll()
{
    NodeItem* nodeItem = getSelectedNodeItem();
    if(nodeItem){
        clearSelection();
        foreach(NodeItem* child, nodeItem->getChildNodeItems()){
            if(child->isVisible()){
                appendToSelection(child, false);
            }
        }
        enableClipboardActions();
    }
}


/**
 * @brief NodeView::undo
 * This undoes the previous triggered action and recenters the
 * view on the same spot when that action was triggered.
 */
void NodeView::undo()
{
    // undo the action
    if(viewMutex.tryLock()) {

        //clearSelection(true,true);
        //clearSelection(true,false);
        setAttributeModel(0, true);
        emit this->view_Undo();
    }
}


/**
 * @brief NodeView::redo
 * This redoes the previous undo action and recenters the
 * view on the same spot when that action was triggered.
 */
void NodeView::redo()
{

    // redo the action
    if(viewMutex.tryLock()) {
        //clearSelection();

        setAttributeModel(0,true);
        emit this->view_Redo();
    }
}


void NodeView::appendToSelection(GraphMLItem *item, bool updateActions)
{
    if(isItemsAncestorSelected(item)){
        return;
    }

    //Unset Items Descendant Items.
    unsetItemsDescendants(item);

    //Set this item as Selected.
    setGraphMLItemSelected(item, true);

    // when an item is selected, do we want to fit it in the view?
    //if (!item->getGraphML()->getDataValue("kind").endsWith("Definitions") ) {
    //    keepSelectionFullyVisible(item);
    //}

    if(updateActions){
        // update enabled states of cut, copy & paste everytime something is selected
        enableClipboardActions();
    }

    // update toolbar selected items
    updateToolbarSelectedItems();
}

void NodeView::removeFromSelection(GraphMLItem *item)
{
    // Dan, I commented this out so that edge items can be deselected.
    // This has allowed the export snippet toolbutton to display in the context toolbar.
    // I'm not sure if that's going to be a problem. Let me know if it breaks something!
    /*if(!isItemsAncestorSelected(item)){
        return;
    }*/
    //Set this item as Selected.
    setGraphMLItemSelected(item, false);

    // added this here because actions weren't being enabled/disabled
    // correctly when selecting/deselting items using the CTRL key
    enableClipboardActions();

    // update toolbar selected items
    updateToolbarSelectedItems();
}

void NodeView::moveSelection(QPointF delta)
{
    if(viewState == VS_MOVING){
        bool canReduceX = true;
        bool canReduceY = true;

        foreach(QString ID, selectedIDs){
            GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
            GraphML* graphml = graphMLItem->getGraphML();

            if(graphml && graphml->isNode()){
                NodeItem* nodeItem = (NodeItem*) graphMLItem;
                QPointF resultingPosition = nodeItem->pos() + delta;

                // added this so that the Definitions containers can be moved outside of the Model
                //if (nodeItem->getNodeKind().endsWith("Definitions")) {
                //    canReduceX = true;
                //    canReduceY = true;
                //    continue;
                //}

                qreal minX = 0;
                qreal minY = 0;
                if(nodeItem->getParentNodeItem()){
                    QRectF gridRect = nodeItem->getParentNodeItem()->gridRect();
                    minX = gridRect.left();
                    minY = gridRect.top();
                }

                if(resultingPosition.x() < minX){
                    canReduceX = false;
                }
                if(resultingPosition.y() < minY){
                    canReduceY = false;
                }
            }
        }

        if(!canReduceX){
            delta.setX(qMax(0.0, delta.x()));
        }

        if(!canReduceY){
            delta.setY(qMax(0.0, delta.y()));
        }

        /*
        // find out if there is an aspect selected
        bool containsAspectEntity = false;
        bool allAspectEntities = true;

        foreach(QString ID, selectedIDs){
            GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
            if (graphMLItem && graphMLItem->isNodeItem()) {
                NodeItem* item = (NodeItem*) graphMLItem;
                if (item->getNodeKind().endsWith("Definitions")) {
                    containsAspectEntity = true;
                } else {
                    allAspectEntities = false;
                }
            }
        }

        // if all selected entities are aspects, move the model item accordingly
        if (allAspectEntities) {
            adjustModelPosition(delta);
            return;
        }

        // if there is at least one aspect selected, don't move any of the selected items
        if (containsAspectEntity) {
            return;
        }*/

        foreach(QString ID, selectedIDs){
            GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
            GraphML* graphml = graphMLItem->getGraphML();

            if(graphml && graphml->isNode()){
                NodeItem* nodeItem = (NodeItem*) graphMLItem;
                if(nodeItem->getParentNodeItem()){
                    nodeItem->getParentNodeItem()->setGridVisible(true);
                }

                nodeItem->adjustPos(delta);
            }
        }
    }
}

void NodeView::resizeSelection(QString ID, QSizeF delta)
{   
    //Cannot resize from any state except None and Resizing.
    if(viewState == VS_RESIZING){
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
        GraphML* graphml = graphMLItem->getGraphML();
        if(graphml && graphml->isNode()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            nodeItem->adjustSize(delta);
        }
    }
}

void NodeView::moveFinished()
{

    foreach(QString ID, selectedIDs){
        GraphMLItem* currentItem = getGraphMLItemFromHash(ID);
        if(currentItem && currentItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) currentItem;
            if(nodeItem->getParentNodeItem()){
                nodeItem->getParentNodeItem()->setGridVisible(false);
            }
            nodeItem->updateModelPosition();
        }
    }

    setState(VS_SELECTED);
}

void NodeView::resizeFinished(QString ID)
{

    GraphMLItem* currentItem = getGraphMLItemFromHash(ID);
    if(currentItem && currentItem->isNodeItem()){
        NodeItem* nodeItem = (NodeItem*) currentItem;
        nodeItem->updateModelSize();
    }
    setState(VS_SELECTED);
}


void NodeView::view_ClearHistory()
{
    emit view_ClearHistoryStates();
    viewCenterPointStack.clear();

    // clear the maps used for moving the view backwards & forwards
    modelPositions.clear();
    centeredRects.clear();
}


/**
 * @brief NodeView::clearSelection
 * This gets called when either the view or the model is pressed.
 * It clears the selection.
 * @param updateTable
 * @param updateDocks
 */
void NodeView::clearSelection(bool updateTable, bool updateDocks)
{
    while (!selectedIDs.isEmpty()) {
        QString currentID = selectedIDs.takeFirst();
        GraphMLItem* currentItem = getGraphMLItemFromHash(currentID);
        if (currentItem) {
            currentItem->setSelected(false);
        }
    }

    if (updateTable) {
        setAttributeModel(0,false);
    }

    //setState(VS_NONE);

    // update menu and toolbar actions
    updateActionsEnabled();

    enableClipboardActions();

    // this stops unnecessary disabling of docks/dock buttons
    // if the call came from a painted node item, just clear the selection
    NodeItem* senderItem = dynamic_cast<NodeItem*>(QObject::sender());
    if (senderItem && senderItem->isPainted()) {
        return;
    }

    if (updateDocks) {
        emit view_nodeSelected();
    }
}


/**
 * @brief NodeView::toolbarClosed
 * This is called when the toolbar is closed by pressing on the view.
 */
void NodeView::toolbarClosed()
{
    toolbarJustClosed = true;
}


/**
 * @brief NodeView::toggleGridLines
 * This is called when the grid lines are turned on/off from the menu.
 * It sends a signal to the node items.
 * @param gridOn
 */
void NodeView::toggleGridLines(bool gridOn)
{
    GRID_LINES_ON = gridOn;
    emit view_toggleGridLines(GRID_LINES_ON);
}


/**
 * @brief NodeView::autoCenterAspects
 * This is called from the MedeaWindow menu.
 * It sets the automatic centering of the view aspects on and off.
 * @param center
 */
void NodeView::autoCenterAspects(bool center)
{
    AUTO_CENTER_ASPECTS = center;
}


/**
 * @brief NodeView::setSelectOnConstruction
 * This is called from the MedeaWindow menu.
 * It sets the selection of newly constucted node on and off.
 * @param select
 */
void NodeView::selectNodeOnConstruction(bool select)
{
    SELECT_ON_CONSTRUCTION = select;
}


void NodeView::selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint)
{
    if(!controller){
        qCritical() << "NodeView::selectedInRubberBand() Has no attached Controller.";
        return;
    }

    Model* model = this->controller->getModel();
    NodeItem* modelItem = 0;

    if(!model){
        qCritical() << "NodeView::selectedInRubberBand() controller has no Model!";
        return;
    }

    modelItem = getNodeItemFromGraphMLItem(getGraphMLItemFromGraphML(model));

    if(!modelItem){
        qCritical() << "NodeView::selectedInRubberBand() NodeView has no GUIItem for Model!";
        return;
    }

    QRectF selectionRectangle(fromScenePoint, toScenePoint);

    QList<NodeItem*> nodeItems;
    nodeItems << modelItem;

    //clearSelection();

    while(nodeItems.size() > 0){
        NodeItem* currentNode = nodeItems.takeFirst();
        if(currentNode->intersectsRectangle(selectionRectangle) && currentNode->isVisible() && currentNode->isPainted()){
            appendToSelection(currentNode);
        }else{
            nodeItems << currentNode->getChildNodeItems();
        }
    }
}


void NodeView::constructGUIItem(GraphML *item){

    Node* node = dynamic_cast<Node*>(item);
    Edge* edge = dynamic_cast<Edge*>(item);


    if(node){
        view_ConstructNodeGUI(node);
    }else if(edge){
        view_ConstructEdgeGUI(edge);
    }else{
        qCritical() << "Unknown Type";
    }

}


void NodeView::destructGUIItem(QString ID, GraphML::KIND kind)
{
    Q_UNUSED(kind);
    removeGraphMLItemFromHash(ID);
}


/**
 * @brief NodeView::showManagementComponents
 * @param show
 */
void NodeView::showManagementComponents(bool show)
{
    Node* assemblyDefinition = 0;
    Model* model = controller->getModel();
    if (model) {
        QList<Node*> result = model->getChildrenOfKind("AssemblyDefinitions");
        if(result.size() == 1){
            assemblyDefinition = result[0];
        }else{
            return;
        }
    }

    numberOfNotifications = 2;
    notificationNumber = 0;

    // make sure that the aspects for Deployment are turned on
    if (show) {
        viewDeploymentAspect();
        notificationNumber++;
    }  else {
        numberOfNotifications = 1;
    }

    /*if (show) {
        view_displayNotification("Displayed Management Components.", notificationNumber, numberOfNotifications);
    } else {
        view_displayNotification("Hidden Management Components.", notificationNumber, numberOfNotifications);
    }*/

    // this goes through all the ManagementComponents and shows/hides them
    foreach (Node* node, assemblyDefinition->getChildren(0)){
        NodeItem* nodeItem = getNodeItemFromNode(node);
        if(nodeItem->getNodeKind() == "ManagementComponent"){
            nodeItem->setHidden(!show);
            nodeItem->setSorted(false);
        }else{
            nodeItem->setSorted(true);
        }
    }

    NodeItem* assemblyNI = getNodeItemFromNode(assemblyDefinition);
    if(assemblyNI){
        assemblyNI->newSort();
    }

    managementComponentVisible = show;

    // reset notification seq and total number
    numberOfNotifications = 1;
    notificationNumber = 0;
}

void NodeView::showLocalNode(bool show)
{
    Node* hardwareDefinition = 0;
    Model* model = controller->getModel();
    if (model){
        QList<Node*> result = model->getChildrenOfKind("HardwareDefinitions");
        if(result.size() == 1){
            hardwareDefinition = result[0];
        }else{
            return;
        }
    }

    // this goes through all the ManagementComponents and shows/hides them
    foreach (Node* node, hardwareDefinition->getChildren(0)){
        NodeItem* nodeItem = getNodeItemFromNode(node);
        if(nodeItem->getNodeKind() == "HardwareNode"){
            nodeItem->setHidden(!show);
            nodeItem->setSorted(false);
        }else{
            nodeItem->setSorted(true);
        }
    }

    NodeItem* hardwareNI = getNodeItemFromNode(hardwareDefinition);
    if(hardwareNI){
        hardwareNI->newSort();
    }

    localNodeVisible = show;
}


/**
 * @brief NodeView::toggleZoomAnchor
 * This slot switches the view's zoom anchor.
 * This is called by one of the menu action's in MEDEA.
 * @param underMouse
 */
void NodeView::toggleZoomAnchor(bool underMouse)
{
    if (underMouse) {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    } else {
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    }
}




/**
 * @brief NodeView::fitToScreen
 * This makes sure that all the visible items fit and are centered within the view.
 * If there is a list provided, only go through the items in the list.
 * Otherwise, go through all the items on the scene.
 * @param itemsToCenter - list of items to center on
 * @param padding - padding around the items rect
 * @param addToMap - detemines whether a rect/pos should be added to the maps
 */
void NodeView::fitToScreen(QList<NodeItem*> itemsToCenter, double padding, bool addToMap)
{
    // if there are no aspects turned on, center on the model item
    if (getModelItem() && currentAspects.count() == 0) {
        centerRect(getModelItem()->sceneBoundingRect(), padding, addToMap);
        return;
    }

    // if there is no list provided, use the full node items list
    if (itemsToCenter.isEmpty()) {
        itemsToCenter = getNodeItemsList();
    }

    QRectF itemsRec = scene()->itemsBoundingRect();
    float leftMostX = itemsRec.bottomRight().x();
    float rightMostX = itemsRec.topLeft().x();
    float topMostY = itemsRec.bottomRight().y();
    float bottomMostY = itemsRec.topLeft().y();

    // go through each item and store the left/right/top/bottom most coordinates
    // of the visible items to create the visible itemsBoundingRect to center on
    foreach (NodeItem* nodeItem, itemsToCenter) {
        if (nodeItem && nodeItem->isPainted() && !nodeItem->isHidden() && nodeItem->isInAspect()) {
            QPointF pf = nodeItem->scenePos();
            if (pf.x() < leftMostX) {
                leftMostX = pf.x();
            }
            if ((pf.x() + nodeItem->boundingRect().width()) > rightMostX) {
                rightMostX = pf.x() + nodeItem->boundingRect().width();
            }
            if (pf.y() < topMostY) {
                topMostY = pf.y();
            }
            if ((pf.y() + nodeItem->boundingRect().height()) > bottomMostY) {
                bottomMostY = pf.y() + nodeItem->boundingRect().height();
            }
        }
    }

    QRectF visibleItemsRec = QRectF(leftMostX, topMostY, fabs((rightMostX-leftMostX)), fabs((bottomMostY-topMostY)));
    centerRect(visibleItemsRec, padding, addToMap);
}


/**
 * @brief NodeView::goToDefinition
 * If the node is a definition, select and center it.
 * If it's not but it has a definition, center on its definition.
 * @param node
 */
void NodeView::goToDefinition(Node *node)
{
    if (!node) {
        node = getSelectedNode();
    }
    if (!node) {
        return;
    }

    /*
    //Node* defn = hasDefinition(node);

    if (defn) {
        // make sure the Definitions view aspect is on
        addAspect("Definitions");

        GraphMLItem* guiItem = getGraphMLItemFromGraphML(defn);
        clearSelection(false);
        appendToSelection(guiItem);
        centerOnItem();
    }*/
}


/**
 * @brief NodeView::goToImplementation
 * If the node is not a definition, check to see if it has a definition.
 * If it does and it has at least 1 implementation, select & center on the first one.
 * @param node
 */
void NodeView::goToImplementation(Node *node)
{
    if (!node) {
        node = getSelectedNode();
    }
    if (!node) {
        return;
    }

    /*
    Node* impl = hasImplementation(node);

    if (impl) {
        // make sure the Workload view aspect is on
        addAspect("Workload");

        GraphMLItem* guiItem = getGraphMLItemFromGraphML(impl);
        clearSelection(false);
        appendToSelection(guiItem);
        centerOnItem();
    }*/
}


/**
 * @brief NodeView::goToInstance
 * Select and center on the provided instance.
 * @param instance
 */
void NodeView::goToInstance(Node *instance)
{
    if (!instance) {
        return;
    }

    // make sure the Assembly view aspect is on
    addAspect("Assembly");

    GraphMLItem* guiItem = getGraphMLItemFromGraphML(instance);
    clearSelection(false);
    appendToSelection(guiItem);
    centerOnItem();
}


/**
 * @brief NodeView::deleteSelection
 * This triggers the same actions as for when the DELETE key is pressed.
 */
void NodeView::deleteSelection()
{
    deleteFromIDs(selectedIDs);
}


/**
 * @brief NodeView::resetModel
 * This method is called after the model is cleared.
 * It resets the size, sorts and centers the model.
 */
void NodeView::resetModel(bool addAction)
{
    if(addAction){
        triggerAction("Resetting Model");
    }
    foreach (NodeItem* nodeItem, getNodeItemsList()) {
        if (nodeItem) {
            nodeItem->resetSize();
        }
    }

    sortModel();
    emit view_ProjectCleared();
}


/**
 * @brief NodeView::clearModel
 * This tells the controller to clear the model.
 * This is called from MedeaWindow.
 */
void NodeView::clearModel()
{
    if(viewMutex.tryLock()){
        clearingModel = true;
        emit view_Clear();
    }
}


/**
 * @brief NodeView::view_SelectModel
 * This is called when the project name is clicked.
 * It selects the model item and updates the data table.
 */
void NodeView::selectModel()
{
    if (getModelItem()) {
        clearSelection();
        appendToSelection(getModelItem());
    }
}


/**
 * @brief NodeView::sortModel
 * This gets the model from the controller and then sorts it.
 * Sorting the model sorts the view aspects.
 */
void NodeView::sortModel()
{
    if (!controller) {
        return;
    }
    if(viewMutex.tryLock()){
        if (getModelItem()) {
            getModelItem()->newSort();
        }
        actionFinished();
    }
}
