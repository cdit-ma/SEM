#include "nodeview.h"
#include "../Controller/controller.h"
#include "Toolbar/toolbarwidget.h"
#include "Dock/docktogglebutton.h"
#include "../medeasubwindow.h"
#include "../medeawindow.h"
#include "GraphicsItems/aspectitem.h"
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
#include <QBitmap>
#include "GraphicsItems/entityitem.h"

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR

#define VIEW_PADDING 1.1
#define CENTER_ON_PADDING 1 / 0.25

#define MAX_ZOOM_RATIO 50
#define MIN_ZOOM_RATIO 2



#define THEME_LIGHT 0
#define THEME_DARK 1

#define ASPECT_INTERFACES "Interfaces"
#define ASPECT_BEHAVIOUR "Behaviour"
#define ASPECT_ASSEMBLIES "Assemblies"
#define ASPECT_HARDWARE "Hardware"

/**
 * @brief NodeView::NodeView
 * @param subView
 * @param parent
 */
NodeView::NodeView(bool subView, QWidget *parent):QGraphicsView(parent)
{
    wasPanning = false;
    connectLine = 0;
    constructedFromImport = true;
    toolbarJustClosed = false;
    editingEntityItemLabel = false;
    managementComponentVisible = false;
    toolbarDockConstruction = false;
    importFromJenkins = false;
    hardwareDockOpen = false;
    showConnectLine = true;
    showSearchSuggestions = false;

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
    // setContextMenuPolicy(Qt::CustomContextMenu);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);


    this->scene()->setItemIndexMethod(QGraphicsScene::NoIndex);


    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Set GraphicsView background-color


    //Sublime
    //setStyleSheet("QGraphicsView{ background-color: rgba(64,64,64,255); border: 0px;}");
    //Google Background
    //setStyleSheet("QGraphicsView{ background-color: rgba(245,245,245,255); border: 0px;}");

    //setStyleSheet("QGraphicsView{ background-color: rgba(190,190,190,255); border: 0px;}");
    //setStyleSheet("QGraphicsView{ background-color: rgba(233,234,237,255); border: 0px;}");
    //setStyleSheet("QGraphicsView{ background-color: rgb(220,220,220); border: 0px;}");
    //setStyleSheet("QGraphicsView{ background-color: rgb(100,100,100); border: 0px;}");

    // this is the original colour
    //setStyleSheet("QGraphicsView{ background-color: rgba(170,170,170,255); border: 0px;}");

    //Set The rubberband Mode.
    setRubberBandMode(false);

    nonDrawnItemKinds << "DeploymentDefinitions";


    // construct toolbar widget
    toolbar = new ToolbarWidget(this);
    if (isMainView()) {
        connect(this, SIGNAL(view_updateMenuActionEnabled(QString,bool)), toolbar, SLOT(updateActionEnabledState(QString,bool)));
        connect(this, SIGNAL(view_themeChanged(int)), toolbar, SLOT(setupTheme(int)));
    }

    comboBox = new QComboBox(this);
    comboBox->setVisible(false);
    comboBox->setFixedHeight(0);
    if(isMainView()){
        connect(comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(dropDownChangedValue(QString)));
    }



    // call this after the toolbar has been constructed to pass on the theme
    setupTheme();
    //setupTheme(THEME_DARK);

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

void NodeView::scrollContentsBy(int dx, int dy)
{
    //QGraphicsView::scrollContentsBy(dx,dy);
}


void NodeView::enforceItemAspectOn(int ID)
{
    GraphMLItem* item = getGraphMLItemFromID(ID);
    if(item && item->isNodeItem()){
        VIEW_ASPECT neededAspect = ((NodeItem*)item)->getViewAspect();
        AspectItem* aspect = getAspectItem(neededAspect);
        if(!aspect->isVisible()){
            emit view_toggleAspect(neededAspect, true);
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

NodeView::~NodeView()
{
    IS_DESTRUCTING = true;
    //Clear the current Selected Attribute Model so that the GUI doesn't crash.
    setAttributeModel(0, true);

    if(parentNodeView && !parentNodeView->isTerminating()){
        parentNodeView->removeSubView(this);

    }
}

void NodeView::setCursor(QCursor cursor)
{
    if(viewport() && viewport()->cursor().shape() != cursor.shape()){
        viewport()->setCursor(cursor);
    }
}

void NodeView::unsetCursor()
{
    if(viewport() && viewport()->cursor().shape() != Qt::ArrowCursor){
        viewport()->unsetCursor();
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
 */
void NodeView::centerRect(QRectF rect, double padding, bool addToMap)
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
    rect.setWidth(rect.width() * padding);
    rect.setHeight(rect.height() * padding);

    if(isSubView()){
        visibleViewRect = viewport()->rect();
    }

    // calculate the ratio (viewport : rect)
    // scale depending on which side requires less scaling
    double widthScale = visibleViewRect.width() / rect.width();
    double heightScale = visibleViewRect.height() / rect.height();
    double newScale = qMin(widthScale, heightScale);

    QPoint centerOffset = viewport()->rect().center() - visibleViewRect.center();
    QPointF sceneOffset = QPointF(centerOffset.x() / newScale, centerOffset.y() / newScale);

    // reset current transform before scaling
    setTransform(QTransform());
    scale(newScale, newScale);

    // center the view on rect's original center
    centerViewOn(rectCenter + sceneOffset);

    // check if any of the aspect toggle buttons need highlighting
    aspectGraphicsChanged();
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
 * This returns the GUI item for the Model.
 * @return
 */
ModelItem *NodeView::getModelItem()
{
    ModelItem* model = 0;
    if(controller){
        int ID = controller->getModel()->getID();
        GraphMLItem* item = getGraphMLItemFromID(ID);
        model = (ModelItem*)item;
    }
    return model;
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
 * @brief NodeView::adjustModelPosition
 * @param delta
 */
void NodeView::adjustModelPosition(QPointF delta)
{
    if (getModelItem()) {
        ((ModelItem*)getModelItem())->adjustPos(delta);
        // call this after the scene/model has been moved
        aspectGraphicsChanged();
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
 * @brief NodeView::getEntityItemsList
 * Return the list of all node items on the scene.
 * @return
 */
QList<EntityItem*> NodeView::getEntityItemsList()
{
    QList<EntityItem*> entityItems;
    foreach (GraphMLItem* item, guiItems) {
        if (item->isEntityItem()) {
            entityItems.append((EntityItem*)item);
        }
    }
    return entityItems;
}


/**
 * @brief NodeView::getNodeItemsList
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

void NodeView::paintEvent(QPaintEvent * e)
{
    QGraphicsView::paintEvent(e);
/*
    if(this->isSubView()){
        QPainter painter(this->viewport());

        painter.setPen(Qt::black);
        painter.drawRect(visibleViewRect);

        painter.setPen(Qt::red);
        painter.drawLine(QLine(visibleViewRect.topLeft(),visibleViewRect.bottomRight()));
        painter.drawLine(QLine(visibleViewRect.topRight(),visibleViewRect.bottomLeft()));

        painter.setPen(Qt::blue);
        painter.drawLine(QLine(viewport()->rect().topLeft(),viewport()->rect().bottomRight()));
        painter.drawLine(QLine(viewport()->rect().topRight(),viewport()->rect().bottomLeft()));
        qCritical() << this->viewState;
    }*/
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
    if (this->hasFocus() && editingEntityItemLabel) {
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


int NodeView::getSelectedNodeID()
{
    if (selectedIDs.size() == 1) {
        int ID = selectedIDs[0];
        GraphMLItem* item = getGraphMLItemFromID(ID);
        if (item && item->isNodeItem()) {
            return item->getID();
        }
    }
    return -1;
}

int NodeView::getSelectedID()
{
    if (selectedIDs.size() == 1) {
        return selectedIDs[0];
    }
    return -1;
}


/**
 * @brief NodeView::getSelectedEntityItem
 * This returns the currently selected node item based on getSelectedNode().
 * @return
 */
EntityItem *NodeView::getSelectedEntityItem()
{
    int ID = getSelectedNodeID();
    if(ID != -1){
        GraphMLItem* item =getGraphMLItemFromID(ID);
        if(item->isEntityItem()){
            return (EntityItem*)item;
        }
    }
    return 0;
}

NodeItem *NodeView::getSelectedNodeItem()
{
    int ID = getSelectedNodeID();
    if(ID != -1){
        GraphMLItem* item =getGraphMLItemFromID(ID);
        if(item && item->isNodeItem()){
            return (EntityItem*)item;
        }
    }
    return 0;

}

GraphMLItem *NodeView::getSelectedGraphMLItem()
{
    int ID = getSelectedID();
    if(ID != -1){
        GraphMLItem* item =getGraphMLItemFromID(ID);
        return item;
    }
    return 0;
}


/**
 * @brief NodeView::getSelectedEntityItems
 * @return
 */
QList<GraphMLItem *> NodeView::getSelectedItems()
{
    QList<GraphMLItem*> selectedItems;
    foreach (int ID, selectedIDs) {
        GraphMLItem* selectedItem = getGraphMLItemFromID(ID);
        if (selectedItem) {
            selectedItems.append(selectedItem);
        }
    }
    return selectedItems;
}

/*
QList<EntityItem *> NodeView::getSelectedEntityItems()
{
    QList<EntityItem*> selectedItems;
    foreach (int ID, selectedIDs) {
        GraphMLItem* selectedItem = getGraphMLItemFromHash(ID);
        if (selectedItem && selectedItem->isEntityItem()) {
            selectedItems.append((EntityItem*)selectedItem);
        }
    }
    return selectedItems;
}*/

QList<int> NodeView::getSelectedNodeIDs()
{

    QList<int> selectedItems;
    foreach (int ID, selectedIDs) {\
        NodeItem* nodeItem = getNodeItemFromID(ID);
        if (nodeItem) {
            selectedItems << ID;
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


/**
 * @brief NodeView::search
 * @param searchString
 * @param dataKeys
 * @param kind
 * @return
 */
QList<GraphMLItem*> NodeView::search(QString searchString, QStringList viewAspects, QStringList entityKinds, QStringList dataKeys, GraphMLItem::GUI_KIND kind)
{
    QList<GraphMLItem*> matchedItems;
    QStringList matchedDataValues;

    // remove white spaces from the start and end of the search string
    searchString = searchString.trimmed();

    if (searchString.isEmpty()) {
        if (showSearchSuggestions) {
            emit view_searchFinished(matchedDataValues);
        }
        return matchedItems;
    }

    // if the searchString doesn't start and end with '*', add '*' to both ends of the string
    // this forces the regex to check containment instead of just catching the exact case
    if (!searchString.startsWith('*') && !searchString.endsWith('*')) {
        searchString = '*' + searchString + '*';
    }

    QList<GraphMLItem*> itemsToSearch;
    if (kind == GraphMLItem::ENTITY_ITEM) {
        itemsToSearch = *reinterpret_cast<QList<GraphMLItem*>*>(&getEntityItemsList());
    } else if (kind == GraphMLItem::NODE_EDGE) {
        itemsToSearch = *reinterpret_cast<QList<GraphMLItem*>*>(&getEdgeItemsList());
    }

    QRegExp regex(searchString, Qt::CaseInsensitive, QRegExp::Wildcard);
    bool allAspects = viewAspects.isEmpty();
    bool allKinds = entityKinds.isEmpty();

    foreach (GraphMLItem* item, itemsToSearch) {

        // check if item's aspect is in the view aspects list
        /*if (!allAspects && ) {

        }*/

        // check if item's kind is in the entity kinds list
        if (!allKinds && !entityKinds.contains(item->getNodeKind())) {
            continue;
        }

        GraphML* gml = item->getGraphML();
        if (!gml) {
            continue;
        }

        // if searchString matches at least one of the values of the provided
        // data keys for the current graphml item, append the item to the list
        foreach (QString key, dataKeys) {
            QString dataVal = gml->getDataValue(key);
            if (dataVal.isEmpty()) {
                continue;
            }
            if (regex.exactMatch(dataVal)) {
                matchedItems.append(item);
                if (!matchedDataValues.contains(dataVal)) {
                    matchedDataValues.append(dataVal);
                }
                break;
            }
        }
    }

    if (showSearchSuggestions) {
        emit view_searchFinished(matchedDataValues);
    }

    return matchedItems;
}


/**
 * @brief NodeView::searchSuggestionsRequested
 * @param searchString
 * @param dataKeys
 */
void NodeView::searchSuggestionsRequested(QString searchString, QStringList viewAspects, QStringList entityKinds, QStringList dataKeys)
{
    showSearchSuggestions = true;
    search(searchString, viewAspects, entityKinds, dataKeys);
    showSearchSuggestions = false;
}


QStringList NodeView::getAdoptableNodeList(int ID)
{
    return controller->getAdoptableNodeKinds(ID);
}


QList<int> NodeView::getConnectableNodes(int ID)
{
    return controller->getConnectableNodes(ID);
}

QList<NodeItem *> NodeView::getConnectableNodeItems(int ID)
{
    QList<NodeItem*> nodeItems;
    QList<int> IDs = controller->getConnectableNodes(ID);
    NodeItem* src = getEntityItemFromID(ID);
    foreach(int cID, IDs){
        NodeItem* entityItem = getEntityItemFromID(cID);

        if(src && entityItem){
            //Ignore edges between AggregateInstance and AggregateInstances in Dock
            if(src->getNodeKind() == "AggregateInstance" && entityItem->getNodeKind() == "AggregateInstance"){
                continue;
            }
        }
        if(entityItem){
            nodeItems.append(entityItem);
        }
    }
    return nodeItems;
}


/**
 * @brief NodeView::getConnectableEntityItems
 * @param IDs
 * @return
 */
QList<NodeItem *> NodeView::getConnectableNodeItems(QList<int> IDs)
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


    QList<EntityItem*> sharedLegalEntityItems;
    foreach (QString hashID, connectableNodesHash.keys()) {
        if (connectableNodesHash[hashID] == IDs.count()) {
            EntityItem* item = getEntityItemFromID(hashID);
            if (item) {
                sharedLegalEntityItems.append(item);
            }
        }
    }

    return sharedLegalEntityItems;
    */

    return QList<NodeItem*>();
}


QList<NodeItem *> NodeView::getNodeInstances(int ID)
{
    QList<NodeItem*> nodeItems;
    if(controller){
        foreach(int iID, controller->getInstances(ID)){
            NodeItem* nodeItem = getEntityItemFromID(iID);
            if(nodeItem){
                nodeItems.append(nodeItem);
            }
        }
    }
    return nodeItems;
}

/**
 * @brief NodeView::getHardwareList
 * @return
 */
QList<NodeItem *> NodeView::getHardwareList()
{
    QList<NodeItem*> hardwares;
    if (controller) {
        Model* model = controller->getModel();
        if (model) {
            QList<Node*> clusters = model->getChildrenOfKind("HardwareCluster");
            foreach (Node* c, clusters) {
                hardwares.append(getEntityItemFromNode(c));
            }
            QList<Node*> nodes = model->getChildrenOfKind("HardwareNode");
            foreach (Node* n, nodes) {
                hardwares.append(getEntityItemFromNode(n));
            }
        }
    }
    return hardwares;
}


/**
 * @brief NodeView::constructNewView
 * This slot is called from the window/context toolbar when the popup new view action is triggered.
 * @param nodeKindToCenter - 0 = selected node
 *                           1 = selected node's definition
 *                           2 = selected node's implementation
 */
void NodeView::constructNewView(int nodeKindToCenter)
{
    if (getSelectedNodeID() == -1) {
        return;
    }

    int nodeID = -1;

    switch (nodeKindToCenter) {
    case -1:
    case 0:
        nodeID = getSelectedNodeID();
        break;
    case -2:
        nodeID = getDefinitionID(nodeID);
        break;
    case -3:
        nodeID = getImplementationID(nodeID);
        break;
    }



    if (nodeID <= 0) {
        qWarning() << "NodeView::constructNewView - nodeID is null.";
        return;
    }

    NodeItem* nodeItem = getNodeItemFromID(nodeID);

    if (IS_SUB_VIEW || !nodeItem){
        return;
    }

    MedeaSubWindow* subWindow = new MedeaSubWindow();
    connect(this, SIGNAL(destroyed()), subWindow, SLOT(reject()));

    NodeView* newView = new NodeView(true , subWindow);
    subViews.append(newView);
    newView->setParentNodeView(this);

    subWindow->setNodeView(newView);
    newView->view_LockCenteredGraphML(nodeID);

    Node* currentNode = nodeItem->getNode();

    if (controller && currentNode) {

        controller->connectView(newView);

        QList<Edge*> edgeList;
        QList<Node*> constructList;

        // get the children
        constructList << currentNode->getChildren();
        edgeList << currentNode->getEdges();

        while (currentNode) {
            // add the parent of the currentNode
            constructList.insert(0,currentNode);
            currentNode = currentNode->getParentNode();
        }
        while (!constructList.isEmpty()) {
            Node* newNode = constructList.takeFirst();
            newView->constructGUIItem(newNode);
        }
        while (!edgeList.isEmpty()) {
            newView->constructGUIItem(edgeList.takeFirst());
        }

        connect(this, SIGNAL(view_ClearSubViewAttributeTable()), newView, SIGNAL(view_ClearSubViewAttributeTable()));
        subWindow->show();
        //Centralize item.
        newView->centralizedItemMoved();
    } else {
        delete subWindow;
    }
}
QList<NodeItem *> NodeView::getEntityItemsOfKind(QString kind, int ID, int depth)
{
    QList<NodeItem*> nodes;

    if(controller){
        foreach(int childID, controller->getNodesOfKind(kind, ID, depth)){
            EntityItem* child = getEntityItemFromID(childID);
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
    emit view_ZoomChanged(transform().m11());


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

    // added this here to re-centralise on the model after import
    if (constructedFromImport) {
        fitToScreen();
        constructedFromImport = false;
    }

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

    // update menu and toolbar actions
    updateActionsEnabledStates();

    viewMutex.unlock();
}

void NodeView::dropDownChangedValue(QString value)
{
    if(comboBox){
        comboBox->setVisible(false);
        GraphMLItem* item = getSelectedGraphMLItem();

        if(item){
            int ID = item->getID();
            QString dataType = getEditableDataKeyName(item).first;
            if(ID > 0){
                emit view_SetGraphMLData(ID, dataType, value);
            }
        }
    }
}

void NodeView::showDropDown(GraphMLItem *item, QLineF dropDownPosition, QString keyName, QString currentValue)
{
    if(comboBox){
        QPoint topLeft = mapFromScene(dropDownPosition.p1());
        QPoint topRight = mapFromScene(dropDownPosition.p2());

        int width = topRight.x() - topLeft.x();


        QStringList validValues = controller->getValidKeyValues(keyName, item->getID());
        comboBox->clear();

        comboBox->addItems(validValues);
        comboBox->setCurrentText(currentValue);
        comboBox->setFixedWidth(width);
        comboBox->move(topLeft);
        comboBox->showPopup();
        comboBox->setVisible(true);
    }
}


/**
 * @brief NodeView::setupTheme
 * @param theme
 */
void NodeView::setupTheme(int theme)
{
    QString background = ";";

    switch (theme) {
    case THEME_LIGHT:
        background = "rgba(170,170,170,255);";
        //background = "white;";
        break;
    case THEME_DARK:
        //background = "rgb(30,30,30);";
        background = "rgb(70,70,70);";
        //background = "rgb(100,100,100);";
        //background = "rgba(70,130,180);";
        //background = "rgba(203,210,212);";
        //background = "black;";
        break;
    default:
        break;
    }

    setStyleSheet("QGraphicsView {"
                  "background-color:" + background +
                  "border: 0px;}");

    currentTheme = theme;
    emit view_themeChanged(theme);
}

/**
 * @brief NodeView::getTheme
 * @return
 */
int NodeView::getTheme()
{
    return currentTheme;
}


/**
 * @brief NodeView::aspectGraphicsChanged
 * This checks if the current viewport is completely contained within one of the dispalyed aspects.
 * If it is, highlight the corresponding aspect's toggle button's label.
 */
void NodeView::aspectGraphicsChanged()
{
    // opening a subview will crash without this check
    if (isSubView()) {
        return;
    }

    if (!getModelItem()) {
        return;
    }

    QRectF viewSceneRect = getVisibleRect();
    VIEW_ASPECT aspectToHighlight = VA_NONE;

    foreach (GraphMLItem* aspect, getModelItem()->getChildren()) {
        if (aspect->isVisible() && aspect->sceneBoundingRect().contains(viewSceneRect)){
            aspectToHighlight = ((NodeItem*)aspect)->getViewAspect();
            break;
        }
    }

    emit view_highlightAspectButton(aspectToHighlight);
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

void NodeView::setStateConnect()
{
    QToolButton* sender = qobject_cast<QToolButton*>(QObject::sender());
    if (sender) {
        showConnectLine = false;
    }
    setState(VS_CONNECT);
}


void NodeView::setStateSelected()
{
    setState(VS_SELECTED);
}

void NodeView::clearState()
{
    setState(VS_NONE);
}


void NodeView::request_ImportSnippet()
{
    GraphMLItem* selectedItem = getSelectedGraphMLItem();

    if (selectedItem){
        emit view_ImportSnippet(selectedItem->getNodeKind());
    }
}


/**
 * @brief NodeView::hardwareDockOpened
 * @param opened
 */
void NodeView::hardwareDockOpened(bool opened)
{
    hardwareDockOpen = opened;
    if (opened) {
        highlightDeployment();
    } else {
        highlightDeployment(true);
    }
}


void NodeView::showQuestion(MESSAGE_TYPE type, QString title, QString message, int ID)
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
        currentTableID = -1;
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
        QList<int> duplicateList = selectedIDs;
        clearSelection();
        emit view_ImportedSnippet(duplicateList, fileName, fileData);
    }
}

void NodeView::scrollEvent(int delta)
{
    if(viewState == VS_NONE || viewState ==  VS_SELECTED || viewState == VS_CONNECT || viewState == VS_CONNECTING){
        QRectF viewRect = viewport()->rect();

        //Turn
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
        // call this after zooming
        aspectGraphicsChanged();
    }
}




void NodeView::minimapPanning(QPointF delta)
{

    if(viewState == VS_PAN){
        setState(VS_PANNING);
    }
    if(viewState == VS_PANNING){
        //translate to scenepos.
        adjustModelPosition(delta);
    }
}

void NodeView::minimapPanned()
{
    if(viewState == VS_PANNING){
        if(selectedIDs.isEmpty()){
            setState(VS_NONE);
        }else{
            setState(VS_SELECTED);
        }
    }
}

void NodeView::minimapScrolled(int delta)
{
    ViewportAnchor currentAnchor = transformationAnchor();
    setTransformationAnchor(AnchorViewCenter);

    scrollEvent(delta);
    setTransformationAnchor(currentAnchor);
}


void NodeView::setRubberBandMode(bool On)
{
    if(isMainView()){
        if(!rubberBand){
            rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        }
        if(On){
            setDragMode(RubberBandDrag);
            rubberBand->resize(0,0);
            rubberBand->setVisible(true);
        }else{
            setDragMode(NoDrag);
            rubberBand->setVisible(false);
        }
    }
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

void NodeView::minimapPan()
{
    if(viewState == VS_NONE || viewState == VS_SELECTED){
        setState(VS_PAN);
    }
}


void NodeView::toggleAspect(VIEW_ASPECT aspect, bool on)
{
    AspectItem* aspectItem = getAspectItem(aspect);
    if(aspectItem){
        aspectItem->setVisible(on);
    }

    if(AUTO_CENTER_ASPECTS) {
        fitToScreen();
    }

    aspectGraphicsChanged();
}


/**
 * @brief NodeView::sortNode
 * This method recursively sorts the provided node.
 * It sorts from the lowest level children back up to the provided node.
 * @param node
 * @param topMostNode
 */
void NodeView::sortSelection(bool recurse)
{
    if(viewMutex.tryLock()){      
        triggerAction("View: Sorting Node");
        emit view_updateProgressStatus(-1, "Sorting Selected Entities");

        QList<GraphMLItem*> nodesToSort;

        QList<GraphMLItem*> sortList;

        foreach(int ID, selectedIDs){
            NodeItem* nodeItem = getNodeItemFromID(ID);
            if(nodeItem){
                nodesToSort << nodeItem;
            }
        }

        //Construct a list of all children.
        while(!nodesToSort.isEmpty()){
            GraphMLItem* child = nodesToSort.takeFirst();
            if(!sortList.contains(child)){
                sortList << child;
            }
            if(recurse){
                nodesToSort.append(child->getChildren());
            }
        }


        while(!sortList.isEmpty()){
            GraphMLItem* child = sortList.takeLast();
            NodeItem* nodeItem = (NodeItem*) child;
            if(child && child->isNodeItem() && child->isVisible()){
                nodeItem->sortChildren();
            }
        }
        update();
        emit view_updateProgressStatus(100);
        actionFinished();
    }


}

void NodeView::expandSelection(bool expand)
{
    if(viewMutex.tryLock()){
        triggerAction("View: Sorting Node");
        emit view_updateProgressStatus(-1, "Expand Selected Entities");

        QList<GraphMLItem*> nodesToSort;

        foreach(int ID, selectedIDs){
            NodeItem* nodeItem = getNodeItemFromID(ID);
            if(nodeItem){
                nodesToSort << nodeItem;
            }
        }

        //Construct a list of all children.
        while(!nodesToSort.isEmpty()){
            GraphMLItem* child = nodesToSort.takeFirst();
            if(child->isEntityItem()){
                EntityItem* entity = (EntityItem*) child;
                if(entity->isExpanded() != expand){
                    entity->setNodeExpanded(expand);
                }
                entity->updateSizeInModel();
            }
        }
        update();
        emit view_updateProgressStatus(100);
        actionFinished();
    }

}


/**
 * @brief NodeView::hardwareClusterMenuClicked
 * @param viewMode
 */
void NodeView::hardwareClusterMenuClicked(int viewMode)
{
    foreach(GraphMLItem* item, getSelectedItems()){
        if(item->isEntityItem()){
            ((EntityItem*)item)->updateChildrenViewMode(viewMode);
        }
    }
}


/**
 * @brief NodeView::centerAspect
 * @param aspect
 */
void NodeView::centerAspect(VIEW_ASPECT aspect)
{
    AspectItem* aspectItem = getAspectItem(aspect);
    if(aspectItem){
        centerOnItem(aspectItem);
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
    if (!item) {
        item = getSelectedGraphMLItem();
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

void NodeView::centerItem(int ID)
{
    GraphMLItem* item = getGraphMLItemFromID(ID);
    centerItem(item);
}

void NodeView::centerDefinition(int ID)
{
    if(ID == -1){
        ID = getSelectedNodeID();
    }

    if(controller && ID != -1){

        int definitionID =  controller->getDefinition(ID);

        GraphMLItem *definition = getEntityItemFromID(definitionID);

        if(definition){
            // make sure the Assembly view aspect is on
            clearSelection(false);
            appendToSelection(definition);
            centerOnItem();
        }
    }
}

void NodeView::centerImplementation(int ID)
{
    if(ID == -1){
        ID = getSelectedNodeID();
    }
    if(controller && ID != -1){
        int implementationID =  controller->getImplementation(ID);
        GraphMLItem *impl = getEntityItemFromID(implementationID);

        if(impl){
            // make sure the Assembly view aspect is on
            clearSelection(false);
            appendToSelection(impl);
            centerOnItem();
        }
    }

}

void NodeView::centerInstance(int instanceID)
{
    if(controller){
        GraphMLItem *instance = getEntityItemFromID(instanceID);

        if(instance){
            // make sure the Assembly view aspect is on
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
    if (!item) {
        item = getSelectedGraphMLItem();
    }

    if (item) {
        enforceItemAspectOn(item->getID());
        // if the selected node is a main container, just use centerItem()
        // we would only ever want to center and zoom into it
        if (item->isAspectItem()) {
            centerItem(item);
        } else {
            centerRect(item->sceneBoundingRect(), CENTER_ON_PADDING, true);
        }
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
    editingEntityItemLabel = hasFocus;
}


/**
 * @brief NodeView::selectAndCenterItem
 * This selects and center on the provided item.
 * If an ID is provided instead of an item, get the corresponding item from the hash.
 * @param item - graphics item to select and center on
 * @param ID - ID of the graphics item to select and center on
 */
void NodeView::selectAndCenterItem(int ID)
{
    GraphMLItem* item = getGraphMLItemFromID(ID);

    if (item) {
        enforceItemAspectOn(ID);
        // make sure that the parent of EntityItem (if there is one) is expanded
        GraphMLItem* parentItem = item->getParent();
        if (parentItem && parentItem->isEntityItem()) {
            EntityItem* parentEntity = (EntityItem*) parentItem;

            // make sure that the parent of nodeItem is expanded
            if (!parentEntity->isExpanded()) {
                parentEntity->setNodeExpanded(true);
            }

            // if it's a HardwareNode, make sure that its parent cluster's view mode is set to ALL
            if (parentEntity->isHardwareCluster()) {
                parentEntity->updateChildrenViewMode(0);
            }
        }

        // clear the selection, select the item and then center on it
        clearSelection();
        appendToSelection(item);
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
        if (item->isEntityItem()) {
            ((EntityItem*)item)->deploymentView(false);
        }
    }

    // check if any ComponentAssemblies, ComponentInstances or ManagementComponents
    // have children deployed to a different node; show red hardware icon
    if (controller) {
        Model* model = controller->getModel();
        if (model) {
            foreach (Node* assm, model->getChildrenOfKind("ComponentAssembly")) {
                EntityItem* assmItem = getEntityItemFromNode(assm);
                if (assmItem) {
                    assmItem->deploymentView(true && !clear);
                }
            }
            foreach (Node* inst, model->getChildrenOfKind("ComponentInstance")) {
                EntityItem* instItem = getEntityItemFromNode(inst);
                if (instItem) {
                    instItem->deploymentView(true && !clear);
                }
            }
            foreach (Node* mngt, model->getChildrenOfKind("ManagementComponent")) {
                EntityItem* mngtItem = getEntityItemFromNode(mngt);
                if (mngtItem) {
                    mngtItem->deploymentView(true && !clear);
                }
            }
        }
    }

    if (clear) {
        prevSelectedNodeID = -1;
        return;
    }

    EntityItem* selectedEntityItem = getSelectedEntityItem();
    if (selectedEntityItem) {
        if (selectedEntityItem->getNodeKind().startsWith("Hardware")) {
            return;
        }
        if (!selectedEntityItem->deploymentView(true, selectedEntityItem).isEmpty()) {
            // if there are higlighted children, display notification
            view_displayNotification("The selected entity has children that are deployed to a different node.");
        }
        prevSelectedNodeID = selectedEntityItem->getID();
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
 * @param EntityItem
 */
void NodeView::showHardwareClusterChildrenViewMenu(EntityItem *EntityItem)
{
    if(onlyHardwareClustersSelected()){
        QMenu* menu = EntityItem->getChildrenViewOptionMenu();
        if (menu) {
            if (menu == prevLockMenuOpened) {
                menu->close();
                prevLockMenuOpened = 0;
            } else {
                QRectF lockRect = EntityItem->geChildrenViewOptionMenuSceneRect();
                QPoint offset(lockRect.width()/5, -lockRect.width()/15);
                QPointF menuPos = mapFromScene(lockRect.bottomLeft() + offset);
                menuPos = mapToGlobal(menuPos.toPoint());
                menu->popup(menuPos.toPoint());
                prevLockMenuOpened = menu;
            }
        }
    }
}


/**
 * @brief NodeView::hardwareClusterChildrenViewMenuClosed
 * @param EntityItem
 */
void NodeView::hardwareClusterChildrenViewMenuClosed(EntityItem *EntityItem)
{
    QPointF viewPos = mapFromScene(EntityItem->geChildrenViewOptionMenuSceneRect().topLeft());
    QPointF globalPos = mapToGlobal(viewPos.toPoint());
    QRectF rect(globalPos, EntityItem->geChildrenViewOptionMenuSceneRect().size()*transform().m11());

    if (!rect.contains(QCursor::pos())) {
        prevLockMenuOpened = 0;
        //emit view_EntityItemLockMenuClosed(EntityItem);
    }
}

void NodeView::itemEntered(int ID, bool enter)
{
    GraphMLItem* current = getGraphMLItemFromID(ID);
    if(current && current->canHover()){
        current->setHovered(enter);

        if(enter){
            GraphMLItem* prev = getGraphMLItemFromID(highlightedID);
            if(prev){
                prev->setHovered(false);
            }

            highlightedID = ID;
        }else{
            highlightedID = -1;
        }
    }
}

AspectItem *NodeView::getAspectItem(VIEW_ASPECT aspect)
{
    if(getModelItem()){
        foreach(GraphMLItem* item, getModelItem()->getChildren()){
            if(item->isAspectItem()){
                if(((AspectItem*)item)->getViewAspect() == aspect){
                    return (AspectItem*)item;
                }
            }
        }
    }
    return 0;
}

QPair<QString, bool> NodeView::getEditableDataKeyName(GraphMLItem *node)
{
    QPair<QString, bool> returnType;
    returnType.first = "";
    returnType.second = false;

    QString nodeKind = node->getNodeKind();


    VIEW_ASPECT aspect = VA_NONE;
    if(node && node->isNodeItem()){
        aspect = ((NodeItem*)node)->getViewAspect();
    }


    QStringList dropdownKinds;
    dropdownKinds << "Attribute" << "Member";

    //QStringList noDropDownKinds;
    //noDropDownKinds << "OutEventPort" << "InEventPort" << "AggregateInstance" << "VectorInstance" << "MemberInstance" << "HardwareNode" << "AttributeInstance"  << "OutEventPortInstance" << "InEventPortInstance";;

    QStringList typeKinds;
    typeKinds << "Attribute" << "Member" << "OutEventPort" << "InEventPort" << "Vector" << "AggregateInstance" << "VectorInstance" << "OutEventPortInstance" << "InEventPortInstance" << "OutEventPortDelegate" << "InEventPortDelegate" << "AttributeImpl" <<"OutEventPortImpl" << "InEventPortImpl" << "ComponentInstance" << "ReturnType";

    QStringList valueKinds;
    valueKinds << "Variable" << "Condition" << "InputParameter";

    if(aspect != VA_BEHAVIOUR){
        typeKinds << "MemberInstance";
    }else{
        valueKinds << "MemberInstance";
    }

    if(aspect == VA_ASSEMBLIES){
        valueKinds << "AttributeInstance";
    }else{
        typeKinds << "AttributeInstance";
    }



    bool useType = typeKinds.contains(nodeKind);
    bool useValue = valueKinds.contains(nodeKind);

    if(useType){
        returnType.first = "type";
    }
    if(useValue){
        returnType.first = "value";
    }


    if(nodeKind == "Aggregate"){
        returnType.first = "sortOrder";
    }
    if(nodeKind == "HardwareNode"){
        returnType.first = "ip_address";
    }

    if(nodeKind == "PeriodicEvent"){
        returnType.first = "frequency";
    }
    if(nodeKind == "Model"){
        returnType.first = "middleware";
    }


    if(dropdownKinds.contains(nodeKind)){
        returnType.second = true;
    }

    return returnType;
}

bool NodeView::isNodeVisuallyConnectable(Node *node)
{
    if(node){
        BehaviourNode* bNode = dynamic_cast<BehaviourNode*>(node);
        if(bNode){
            if(!bNode->isUnconnectable()){
                return true;
            }
        }else{
            QString nodeKind = node->getNodeKind();
            if(nodeKind.contains("EventPort")){
                if(nodeKind.endsWith("Delegate")  || nodeKind.endsWith("Instance")){
                    return true;
                }
            }
        }
    }
    return false;
}

bool NodeView::onlyHardwareClustersSelected()
{
    //Only allow menu to show when we only have Hardware Clusters selected.
    foreach(int ID, selectedIDs){
        EntityItem* EntityItem = getEntityItemFromID(ID);
        if(!(EntityItem && EntityItem->getNodeKind() == "HardwareCluster")){
            return false;
        }
    }
    return !selectedIDs.isEmpty();
}

/**
 * @brief NodeView::handleSelection - Called my MouseHandlers to change the selection state.
 * @param item - The item to change selection of.
 * @param setSelected - Should the item be selected.
 * @param controlDown - Is Control Pressed.
 */
void NodeView::handleSelection(GraphMLItem *item, bool setSelected, bool controlDown)
{
    if(!item){
        return;
    }

    if(item->isSelected() && controlDown){
        //DeSelect on control click.
        setSelected = false;
    }

    if(item->isSelected() != setSelected){
        if(setSelected && !controlDown){
            //If we are to select, but control isn't down, we should clear.
            clearSelection();
        }

        if(setSelected){
            //Append to selection
            appendToSelection(item);
        }else{
            //Remove from Selection.
            removeFromSelection(item);
        }
    }
}

void NodeView::setState(NodeView::VIEW_STATE state)
{
    //Only let through VS_NONE, VS_SELECTED and VS_PANNING, otherwise, use VS_NONE.
    if(isSubView() && !(state ==  VS_SELECTED || state == VS_PANNING || state == VS_PAN || state == VS_NONE)){
        state = VS_NONE;
    }

    VIEW_STATE oldState = viewState;

    switch(viewState){
    case VS_NONE:
        //Go onto VS_SELECTED
    case VS_SELECTED:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_MOVING || state == VS_RESIZING || state == VS_PAN || state == VS_RUBBERBAND || state == VS_CONNECT){
            viewState = state;
        }
        break;
    case VS_MOVING:
        //Go onto VS_RESIZING state
    case VS_RESIZING:
        if(state == VS_NONE || state == VS_SELECTED){
            viewState = state;
        }
        break;
    case VS_RUBBERBAND:
        //Go onto VS_RUBBERBANDING state
    case VS_RUBBERBANDING:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_RUBBERBAND || state == VS_RUBBERBANDING){
            viewState = state;
        }
        break;
    case VS_PAN:
        //Go Onto VS_PANNING state
    case VS_PANNING:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_PANNING){
            viewState = state;
        }
        break;
    case VS_CONNECT:
    case VS_CONNECTING:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_CONNECTING){
            viewState = state;
        }
        break;
    default:
        break;
    }

    //Transition
    if(viewState != oldState){
        transition();
    }
}

/**
 * @brief NodeView::transition - Called when viewstate changes.
 */
void NodeView::transition()
{
    switch(viewState){
    case VS_NONE:

        //Do the VS_SELECTED case.
    case VS_SELECTED:
        setConnectMode(false);
        setRubberBandMode(false);
        unsetCursor();
        break;
    case VS_MOVING:
        triggerAction("Moving Selection");
        setCursor(Qt::SizeAllCursor);
        break;
    case VS_RESIZING:
        triggerAction("Resizing Selection");
        //Cursor is set by EntityItem
        break;
    case VS_PAN:
        setCursor(Qt::ClosedHandCursor);
        break;
    case VS_PANNING:
        wasPanning = true;
        break;
    case VS_RUBBERBAND:
        setRubberBandMode(true);
        setCursor(Qt::CrossCursor);
        break;
    case VS_CONNECT:
        setConnectMode(true);
        setCursor(Qt::CrossCursor);
        break;
    case VS_CONNECTING:
        setCursor(Qt::CrossCursor);
        break;
    case VS_RUBBERBANDING:
        break;
    default:
        break;
    }
}


void NodeView::_deleteFromIDs(QList<int> IDs)
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
 * @brief NodeView::updateActionsEnabledStates
 */
void NodeView::updateActionsEnabledStates()
{
    int selectedID = getSelectedNodeID();
    int defnID;
    int implID;
    bool canExport = false;
    bool canImport = false;

    if (controller) {
        defnID = controller->getDefinition(selectedID);
        implID = controller->getImplementation(selectedID);
        canExport = controller->canExportSnippet(selectedIDs);
        canImport = controller->canImportSnippet(selectedIDs);

        emit view_updateMenuActionEnabled("cut", controller->canCut(selectedIDs));
        emit view_updateMenuActionEnabled("copy", controller->canCopy(selectedIDs));
        emit view_updateMenuActionEnabled("replicate", controller->canReplicate(selectedIDs));
        emit view_updateMenuActionEnabled("paste", controller->canPaste(selectedIDs));
        emit view_updateMenuActionEnabled("delete", controller->canDelete(selectedIDs));
        emit view_updateMenuActionEnabled("undo", controller->canUndo());
        emit view_updateMenuActionEnabled("redo", controller->canRedo());
        emit view_updateMenuActionEnabled("localDeployment", controller->canLocalDeploy());

    }
    emit view_updateMenuActionEnabled("sort", !getSelectedNodeIDs().isEmpty());

    emit view_updateMenuActionEnabled("singleSelection", selectedID != -1);
    emit view_updateMenuActionEnabled("multipleSelection", !getSelectedNodeIDs().isEmpty());

    // update other menu actions and toolbar buttons
    emit view_updateMenuActionEnabled("definition", defnID != -1);
    emit view_updateMenuActionEnabled("implementation", implID != -1);
    emit view_updateMenuActionEnabled("exportSnippet", canExport);
    emit view_updateMenuActionEnabled("importSnippet", canImport);
}


/**
 * @brief NodeView::showToolbar
 * This is called when there is a mouse right-click event.
 * It shows/hides the context toolbar depending on where the right click happened.
 * @param position
 */
void NodeView::showToolbar(QPoint position)
{

    if(wasPanning){
        //If we have panned, we shouldn't show the toolbar.
        wasPanning = false;
        return;
    }


    // only show the toolbar if there is at least one grapml item selected
    if (viewState == VS_SELECTED) {

        QList<NodeItem*> selectedNodeItems;
        QList<EdgeItem*> selectedEdgeItems;

        // use mouse click position when constructing node items from toolbar
        QPointF globalPos = mapToGlobal(position);
        toolbarPosition = mapToScene(position);
        bool toolbarPositionContained = false;

        // filter selected items into their corresponding lists
        foreach (int ID, selectedIDs) {
            GraphMLItem* item = getGraphMLItemFromID(ID);

            if (item->isNodeItem()){
                NodeItem* nodeItem = (NodeItem*)item;

                if (item->sceneBoundingRect().contains(toolbarPosition)) {
                    toolbarPositionContained = true;
                }
                selectedNodeItems.append(nodeItem);
            } else if (item->isEdgeItem()) {
                EdgeItem* edgeItem = (EdgeItem*) item;
                if (item->isVisible()) {
                    selectedEdgeItems.append(edgeItem);
                }
            }
        }

        // this case should never be true
        if (selectedNodeItems.isEmpty() && selectedEdgeItems.isEmpty()) {
            qWarning() << "NodeView::showToolbar - There are no selected items.";
            return;
        }

        // eventFromEdgeItem is true when the right-click happened inside an edge item
        if (!selectedEdgeItems.isEmpty() && eventFromEdgeItem) {
            toolbarPositionContained = true;
            eventFromEdgeItem = false;
        }

        // this case happens when this is called from the window toolbar
        // show the toolbar positioned at the first selected item's center
        if (!selectedNodeItems.isEmpty() && position.isNull()) {
            QPointF itemScenePos = selectedNodeItems.at(0)->sceneBoundingRect().center();
            globalPos = mapFromScene(itemScenePos);
            globalPos = mapToGlobal(globalPos.toPoint());
            toolbarPosition = itemScenePos;
            toolbarPositionContained = true;
        }

        // only show the toolbar if the right-click happened inside one of the selected items
        if (toolbarPositionContained) {
            toolbar->move(globalPos.toPoint());
            toolbar->updateToolbar(selectedNodeItems, selectedEdgeItems);
            toolbar->setVisible(true);
        }
    }
}


void NodeView::view_ConstructNodeGUI(Node *node)
{
    if(!node){
        return;
    }

    Node* parentNode = node->getParentNode();

    QString nodeKind = node->getDataValue("kind");

    //Check if we should construct this Node.
    if(nonDrawnItemKinds.contains(nodeKind)){
        noGuiIDHash[node->getID()] = nodeKind;
        return;
    }


    GraphMLItem* parentItem = 0;


    Node* parent = parentNode;

    //Get the topmost visible parent
    while(parent){
        parentItem = getGraphMLItemFromID(parent->getID());
        if(parentItem){
            break;
        }
        parent = parent->getParentNode();
    }

    if(!parentItem && nodeKind != "Model"){
        qCritical() << "NodeView::view_ConstructNodeGUI() SUB_VIEW probably not meant to build this item as we don't have it's parent.";
        return;
    }

    NodeItem* parentNodeItem = 0;
    EntityItem* parentEntityItem = 0;
    if(parentItem && parentItem->isEntityItem()){
        parentEntityItem = (EntityItem*)parentItem;
    }
    if(parentItem && parentItem->isNodeItem()){
        parentNodeItem = (NodeItem*)parentItem;
    }

    //Expanded Parent.
    if(toolbarDockConstruction || importFromJenkins){
        if(parentEntityItem && !parentEntityItem->isExpanded()){
            parentEntityItem->setNodeExpanded(true);
        }
    }


    //Check for Pasted data, which will have x,y set to -1
    qreal xVal = -1;
    qreal yVal = -1;

    GraphMLData* xData = node->getData("x");
    GraphMLData* yData = node->getData("y");

    if(xData && xData->gotDoubleValue()){
        xVal = xData->getDoubleValue();
    }

    if(yData && yData->gotDoubleValue()){
        yVal = yData->getDoubleValue();
    }


    if(xVal == -1 && yVal == -1){
        GraphMLData* wData = node->getData("width");
        GraphMLData* hData = node->getData("height");
        qreal wVal = -1;
        qreal hVal = -1;
        if(wData && wData->gotDoubleValue()){
            wVal = wData->getDoubleValue();
        }
        if(hData && hData->gotDoubleValue()){
            hVal = hData->getDoubleValue();
        }

        QPointF newPosition;
        if(parentNodeItem){
            QRectF itemRect;
            itemRect.setWidth(wVal);
            itemRect.setHeight(hVal);
            newPosition = parentNodeItem->getNextChildPos(itemRect);
        }
        xData->setValue(QString::number(newPosition.x()));
        yData->setValue(QString::number(newPosition.y()));
    }

    NodeItem* item =0 ;
    if(nodeKind == "Model"){
        ModelItem* modelItem = new ModelItem(node);
        storeGraphMLItemInHash(modelItem);
        modelItem->setNodeView(this);


        //Connect the Generic Functionality.
        connectGraphMLItemToController(modelItem);

        // send/do necessary signals/updates when a node has been constructed
        //nodeConstructed_signalUpdates(modelItem);
        // add the newly constructed node item to the scene
        if(!scene()->items().contains(modelItem)){
            scene()->addItem(modelItem);
        }

    }else if(node->isAspect()){
        VIEW_ASPECT aspect = GET_ASPECT_FROM_KIND(nodeKind);

        AspectItem* aspectItem = new AspectItem(node, getModelItem(), aspect);
        item = aspectItem;

    }else{
        item =  new EntityItem(node, parentNodeItem, IS_SUB_VIEW);
    }

    if(item && node){
        NodeItem* entityItem = item;

        if(entityItem->isEntityItem()){
            ((EntityItem*)entityItem)->setNodeConnectable(isNodeVisuallyConnectable(node));
        }



        QPair<QString, bool> editField = getEditableDataKeyName(entityItem);

        if(item->isEntityItem()){
            ((EntityItem*)entityItem)->setEditableField(editField.first, editField.second);
        }

        entityItem->setNodeView(this);
        storeGraphMLItemInHash(entityItem);


        //Connect the Generic Functionality.
        connectGraphMLItemToController(entityItem);

        // send/do necessary signals/updates when a node has been constructed
        nodeConstructed_signalUpdates(entityItem);

        // if SELECT_ON_CONSTRUCTION, select node after construction and center on it
        // the node's label is automatically selected and editable
        if (toolbarDockConstruction && SELECT_ON_CONSTRUCTION) {
            clearSelection(true, false);
            // why not update menu/toolbar actions here?
            appendToSelection(entityItem, false);
            //entityItem->setNewLabel();
            centerOnItem();
            toolbarDockConstruction = false;
        }

        // add the newly constructed node item to the scene
        if(!scene()->items().contains(entityItem)){
            scene()->addItem(entityItem);
        }


        if(isSubView() && item->getID() == centralizedItemID){
            appendToSelection(item);
        }
    }
}


void NodeView::view_ConstructEdgeGUI(Edge *edge)
{

    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    if(isSubView()){
        NodeItem* nodeItem = getNodeItemFromID(centralizedItemID);
        if(nodeItem){
            Node* subViewNode = nodeItem->getNode();
            if(!(subViewNode->isAncestorOf(src) && subViewNode->isAncestorOf(dst))){
                return;
            }
        }
    }


    EntityItem* srcGUI = getEntityItemFromGraphMLItem(getGraphMLItemFromID(src->getID()));
    EntityItem* dstGUI = getEntityItemFromGraphMLItem(getGraphMLItemFromID(dst->getID()));

    if (edge->isDeploymentLink()) {
        updateDeployment = true;
    }
    //edgeConstructed_signalUpdates();

    if(srcGUI != 0 && dstGUI != 0){
        // send necessary signals when an edge has been constucted
        //We have valid GUI elements for both ends of this edge.
        bool constructEdge = true;

        if(edge->isAggregateLink() || edge->isDeploymentLink() || edge->isInstanceLink() || edge->isImplLink()){
            if(edge->isInstanceLink() || edge->isImplLink()){
                srcGUI->updateDefinition();
                definitionIDs[edge->getID()] = src->getID();
            }
            constructEdge = false;
        }


        if(edge->isAssemblyLink() || edge->isDelegateLink() || edge->isComponentLink()){
            constructEdge = true;
        }

        if(!constructEdge){
            return;
        }


        EntityItem* parent = getSharedEntityItemParent(srcGUI, dstGUI);

        if(!parent){
            //GETTING MODEL!?
            qCritical() << "using Model.";
            return;
            //parent = getModelItem();
        }

        //Construct a new GUI Element for this edge.
        EdgeItem* nodeEdge = new EdgeItem(edge,parent, srcGUI, dstGUI);
        //EdgeItem2* nodeEdge2 = new EdgeItem2(edge,parent, srcGUI, dstGUI);


        nodeEdge->setNodeView(this);
        //Add it to the list of EdgeItems in the Model.
        storeGraphMLItemInHash(nodeEdge);
        //storeGraphMLItemInHash(nodeEdge2);

        //connectGraphMLItemToController(nodeEdge);
        connectGraphMLItemToController(nodeEdge);

        /*
        // show hidden hardware node before the new edge is added to scene
        // sort its parent so that the newly visible hardware node can be easily seen
        if (dstGUI->getGraphML()->getDataValue("kind") == "HardwareNode") {
            dstGUI->setHidden(false);
            if (dstGUI->getParentEntityItem()) {
                dstGUI->getParentEntityItem()->newSort();
            }
        }
        */


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


void NodeView::view_CenterGraphML(GraphML *graphML)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    if(guiItem){
        centerItem(guiItem);
    }
}

void NodeView::view_LockCenteredGraphML(int ID)
{
    centralizedItemID = ID;
    CENTRALIZED_ON_ITEM = true;
}

void NodeView::sort()
{
    sortSelection(false);
}

void NodeView::expand(bool expand)
{
    expandSelection(expand);
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
                emit view_ConstructNode(selectedItem->getID(), nodeKind, selectedItem->getNextChildPos());
            } else if (sender == 1) {
                // if from toolbar, place at closest grid point to the toolbar's position
                QPointF position = selectedItem->mapFromScene(toolbarPosition);
                QPointF newPosition = selectedItem->getClosestGridPoint(position);

                emit view_ConstructNode(selectedItem->getID(), nodeKind, newPosition);
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
void NodeView::constructEdge(int srcID, int dstID)
{
    if (viewMutex.tryLock()) {
        triggerAction("Dock/Toolbar: Constructing Edge");
        emit view_ConstructEdge(srcID, dstID, true);
    }
}


void NodeView::destructEdge(int srcID, int dstID, bool triggerAction)
{
    Q_UNUSED(triggerAction);

    if(viewMutex.tryLock()){
        EntityItem* srcNode = getEntityItemFromID(srcID);
        EntityItem* dstNode = getEntityItemFromID(dstID);
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
void NodeView::constructDestructEdges(QList<int> srcIDs, int dstID)
{
    if (viewMutex.tryLock()) {

        // added this for connecting multiple selection using the toolbar
        if (srcIDs.isEmpty()) {
            srcIDs = getSelectedNodeIDs();
        }

        if (!srcIDs.isEmpty()) {
            EntityItem* dstNode = getEntityItemFromID(dstID);
            triggerAction("Dock: Destructing/Constructing Multiple Edges");
            emit view_constructDestructEdges(srcIDs, dstID);
        }

    }
}


void NodeView::deleteFromIDs(QList<int> IDs)
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
void NodeView::constructConnectedNode(int parentID, int dstID, QString kind, int sender)
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
        }else{
            //If we don't have a node Item, forwhatever reason release the mutex.
            viewMutex.unlock();
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

    if(controller){
        int ID = getSelectedID();
        QList<GraphMLItem*> connectedItems;

        foreach(int cID, controller->getConnectedNodes(ID)){
            GraphMLItem* item = getGraphMLItemFromID(cID);
            if(item){
                connectedItems << item;
                appendToSelection(item);
            }
        }
        if(!connectedItems.isEmpty()) {
            fitToScreen(connectedItems, 0);//CONNECTIONS_PADDING);
        }
    }
}





void NodeView::setGraphMLItemSelected(GraphMLItem *item, bool setSelected)
{

    int itemID = item->getID();

    if(setSelected){
        if(!selectedIDs.contains(itemID)){
            int nodeSize = 0;

            GraphML* graphml = item->getGraphML();
            if(graphml && graphml->isNode()){
                nodeSize = ((Node*)graphml)->getTreeIndex().size();
            }

            //Find spot for selectedID;
            int position = selectedIDs.count();
            for(int i = 0;i < selectedIDs.count();i++){
                GraphMLItem* selectedItem = getGraphMLItemFromID(selectedIDs[i]);
                GraphML* graphml = selectedItem->getGraphML();

                if(graphml && graphml->isNode()){
                    int currentPos = ((Node*)graphml)->getTreeIndex().size();
                    if(nodeSize > currentPos){
                        position = i;
                        break;
                    }
                }
            }
            selectedIDs.insert(position, itemID);
            item->setSelected(true);

            if(graphml && graphml->isNode()){
                nodeSelected_signalUpdates();
            }

        }
    }else{
        if(selectedIDs.contains(itemID)){
            setAttributeModel(0);
            selectedIDs.removeAll(itemID);

            item->setSelected(false);

            GraphML* graphml = item->getGraphML();
            if(graphml && graphml->isNode()){
                nodeSelected_signalUpdates();
            }

        }
    }

    if(selectedIDs.count() == 1){
        GraphMLItem* item = getGraphMLItemFromID(selectedIDs.last());
        if(item){
            setAttributeModel(item);
            return;
        }
    }
    if(selectedIDs.isEmpty()){
        if(viewState == VS_SELECTED){
            setState(VS_NONE);
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

void NodeView::visibleViewRectChanged(QRect rect)
{
    visibleViewRect = rect;
}


/**
 * @brief NodeView::getVisibleViewRect
 * @return
 */
QRect NodeView::getVisibleViewRect()
{
    return visibleViewRect;
}



/**
 * @brief NodeView::viewDeploymentAspect
 * This makes sure that the view aspects needed for Deployment are turned on.
 */
void NodeView::viewDeploymentAspect()
{
    // Turn on Aspects
    emit view_toggleAspect(VA_ASSEMBLIES, true);
    emit view_toggleAspect(VA_HARDWARE, true);
    // only show a notification if there has been a change in view aspects
    emit view_displayNotification("Turned on Deployment view aspects.",  notificationNumber, numberOfNotifications);
}

QPixmap NodeView::getImage(QString alias, QString imageName)
{
    QString longName = alias + "/" + imageName;
    if(imageLookup.contains(longName)){
        return imageLookup[longName];
    }else{
        QImage image(":/" + longName + ".png");

        QPixmap imageData = QPixmap::fromImage(image);

        if(alias == "Actions" || alias == "Data"){
            QColor tint;

            if(!tint.isValid()){
                tint = QColor(60, 60, 60, 255);
                if(imageName == "Warning"){
                    tint = QColor(255, 0, 0, 255);
                }
            }

            if(image.size() == QSize(96,96)){
                // morph it into a grayscale image
                image = image.alphaChannel();


                // now replace the colors in the image
                for(int i = 0; i < image.colorCount(); ++i) {
                    tint.setAlpha(qGray(image.color(i)));
                    image.setColor(i, tint.rgba());
                }
                imageData = QPixmap::fromImage(image);
            }
        }


        imageLookup[longName] = imageData;

        return imageData;
    }
}

EntityItem *NodeView::getImplementation(int ID)
{
    EntityItem* impl = 0;
    if(controller){
        int implID = controller->getImplementation(ID);
        impl = getEntityItemFromID(implID);
    }
    return impl;
}

QList<EntityItem *> NodeView::getInstances(int ID)
{
    QList<EntityItem*> instances;
    if(controller){
        foreach(int instanceID, controller->getInstances(ID)){
            EntityItem* instance = getEntityItemFromID(instanceID);
            if(instance){
                instances.append(instance);
            }
        }
    }
    return instances;
}

EntityItem *NodeView::getDefinition(int ID)
{
    EntityItem* def = 0;
    if(controller){
        int defID = controller->getDefinition(ID);
        def = getEntityItemFromID(defID);
    }
    return def;
}

int NodeView::getDefinitionID(int ID)
{
    if(controller){
        return controller->getDefinition(ID);
    }
    return -1;
}


/**
 * @brief NodeView::getImplementationID
 * @param ID
 * @return
 */
int NodeView::getImplementationID(int ID)
{
    if (controller) {
        return controller->getImplementation(ID);
    }
    return -1;
}
EntityItem *NodeView::getAggregate(int ID)
{
    EntityItem* aggr = 0;
    if(controller){
        int aggID = controller->getAggregate(ID);
        aggr = getEntityItemFromID(aggID);
    }
    return aggr;
}

EntityItem *NodeView::getDeployedNode(int ID)
{
    EntityItem* hardwareItem = 0;
    if(controller){
        int deplyID = controller->getDeployedHardwareID(ID);
        hardwareItem = getEntityItemFromID(deplyID);
    }
    return hardwareItem;
}


void NodeView::connectGraphMLItemToController(GraphMLItem *item)
{
    if(!item){
        return;
    }

    ModelItem* modelItem = (ModelItem*)item;
    NodeItem* nodeItem = (NodeItem*)item;
    EdgeItem* edgeItem = (EdgeItem*)item;
    EdgeItem* entityItem = (EdgeItem*)item;

    //All Cases.
    connect(item, SIGNAL(GraphMLItem_ClearSelection(bool)), this, SLOT(clearSelection(bool)));
    connect(item, SIGNAL(GraphMLItem_AppendSelected(GraphMLItem*)), this, SLOT(appendToSelection(GraphMLItem*)));
    connect(item, SIGNAL(GraphMLItem_RemoveSelected(GraphMLItem*)), this, SLOT(removeFromSelection(GraphMLItem*)));
    connect(item, SIGNAL(GraphMLItem_Hovered(int,bool)), this, SLOT(itemEntered(int,bool)));

    connect(this, SIGNAL(view_ZoomChanged(qreal)), item, SLOT(zoomChanged(qreal)));

    if(item->isEdgeItem()){
        connect(edgeItem, SIGNAL(edgeItem_eventFromItem()), this, SLOT(setEventFromEdgeItem()));
    }

    if(item->isModelItem()){
        connect(modelItem, SIGNAL(GraphMLItem_PositionChanged()), this, SIGNAL(view_ModelSizeChanged()));
        //connect(this, SIGNAL(view_themeChanged(int)), entityItem, SLOT(themeChanged(int)));
    }

    if(item->isNodeItem()){
        connect(nodeItem, SIGNAL(NodeItem_ResizeSelection(int, QSizeF)), this, SLOT(resizeSelection(int, QSizeF)));
        connect(nodeItem, SIGNAL(NodeItem_ResizeFinished(int)), this, SLOT(resizeFinished(int)));

        connect(this, SIGNAL(view_toggleGridLines(bool)), nodeItem, SLOT(toggleGridMode(bool)));
    }

    if(isMainView()){
        connect(item, SIGNAL(GraphMLItem_SetCentered(GraphMLItem*)), this, SLOT(centerItem(GraphMLItem*)));
        connect(item, SIGNAL(GraphMLItem_TriggerAction(QString)),  this, SLOT(triggerAction(QString)));
        connect(item, SIGNAL(GraphMLItem_SetGraphMLData(int,QString,QString)), this, SIGNAL(view_SetGraphMLData(int,QString,QString)));
        connect(item, SIGNAL(GraphMLItem_DestructGraphMLData(GraphML*,QString)), this, SIGNAL(view_DestructGraphMLData(GraphML*,QString)));
        connect(item, SIGNAL(GraphMLItem_ConstructGraphMLData(GraphML*,QString)), this, SIGNAL(view_ConstructGraphMLData(GraphML*,QString)));


        if(item->isEntityItem()){
            connect(entityItem, SIGNAL(EntityItem_MoveSelection(QPointF)), this, SLOT(moveSelection(QPointF)));
            connect(entityItem, SIGNAL(EntityItem_MoveFinished()), this, SLOT(moveFinished()));
            connect(entityItem, SIGNAL(EntityItem_HardwareMenuClicked(int)), this, SLOT(hardwareClusterMenuClicked(int)));
            connect(entityItem, SIGNAL(EntityItem_HasFocus(bool)), this, SLOT(editableItemHasFocus(bool)));
            connect(entityItem, SIGNAL(EntityItem_ShowHardwareMenu(EntityItem*)), this, SLOT(showHardwareClusterChildrenViewMenu(EntityItem*)));
            connect(entityItem, SIGNAL(EntityItem_lockMenuClosed(EntityItem*)), this, SLOT(hardwareClusterChildrenViewMenuClosed(EntityItem*)));

            connect(this, SIGNAL(view_edgeConstructed()), entityItem, SLOT(updateChildrenViewMode()));
        }
    }

    if(isSubView()){
        if(CENTRALIZED_ON_ITEM && (item->getID() == centralizedItemID)){
            connect(item, SIGNAL(GraphMLItem_SizeChanged()), this, SLOT(centralizedItemMoved()));
            connect(item, SIGNAL(GraphMLItem_PositionChanged()), this, SLOT(centralizedItemMoved()));
        }
    }
}



EntityItem *NodeView::getEntityItemFromGraphMLItem(GraphMLItem *item)
{
    if(item && item->isEntityItem()){
        return (EntityItem*) item;
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
        int ID = graphML->getID();
        if(guiItems.contains(ID)){
            qCritical() << "Hash already contains GraphMLItem with ID: " << ID;
        }else{
            guiItems[ID] = item;
        }
    }

}

bool NodeView::removeGraphMLItemFromHash(int ID)
{
    bool removed = false;
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
            if(item->isEdgeItem()){
                    EdgeItem* edgeItem = (EdgeItem*)item;
                    updateDeployment = true;
                    if(edgeItem->getSource() && edgeItem->getDestination()){
                        emit view_edgeDeleted(edgeItem->getSource()->getID(), edgeItem->getDestination()->getID());
                    }
            }else{
                if(item->getParent()){
                    emit view_nodeDeleted(item->getID(), item->getParent()->getID());
                }
            }

            item->detach();
            delete item;
        }
        if(IS_SUB_VIEW){
            if(CENTRALIZED_ON_ITEM && centralizedItemID == ID){
                //Delete the nodeView
                deleteLater();
            }
        }
        removed = true;
    }else{
        removed = false;
    }

    //Maybe have important thing.
    if(definitionIDs.contains(ID)){
        EntityItem* src = getEntityItemFromID(definitionIDs[ID]);
        if(src){
            src->updateDefinition();
        }
        definitionIDs.remove(ID);
    }

    return false;
}


/**
 * @brief NodeView::nodeConstructed_signalUpdates
 * This is called whenever a node is constructed.
 * It sends signals to update whatever needs updating.
 * @param EntityItem
 */
void NodeView::nodeConstructed_signalUpdates(NodeItem* entityItem)
{
    // update the docks
    emit view_nodeConstructed(entityItem);

    // this will set the correct theme for the necessary parts of particular EntityItems
    //entityItem->themeChanged(currentTheme);

    // send specific current view states to the newly constaructed node item
    entityItem->toggleGridMode(GRID_LINES_ON);


    if (entityItem->getNodeKind().startsWith("Hardware")) {

        // this will update the HardwareClusters' children view mode
        // and the HardwareNodes' initial visibility
        updateDeployment = true;

    } else if (entityItem->getNodeKind() == "ManagementComponent") {

        bool show = managementComponentsShown();
        if (isSubView() && parentNodeView) {
            show = parentNodeView->managementComponentsShown();
        }
        //entityItem->setHidden(!show);

    } else if (entityItem->getNodeKind() == "AggregateInstance") {

        // hide all AggregateInstances except for in OutEventPortImpls
        NodeItem* parentItem = entityItem->getParentNodeItem();

        if (parentItem) {
            QStringList shownKinds;
            shownKinds << "AggregateInstance" << "Aggregate" << "OutEventPortImpl" << "InEventPortImpl";
            if (!shownKinds.contains(parentItem->getNodeKind()) ){
             //   entityItem->setHidden(true);
            }
        }

    }

    // if currently pasting, select pasted item
    if (pasting || importFromJenkins) {
        appendToSelection(entityItem);
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
    // update the highlighted deployment nodes.
    if (hardwareDockOpen) {
        highlightDeployment();
    }
    // update the docks
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
    if (hardwareDockOpen) {
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
void NodeView::edgeDestructed_signalUpdates(Edge* edge, int ID)
{
    // update the toolbar/menu goTo functions
    //emit view_edgeDestructed();

    // check if destructed edge's destination is a HardwareNode
    EntityItem* destination = getEntityItemFromNode(edge->getDestination());
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


bool NodeView::isItemsAncestorSelected(GraphMLItem *selectedItem)
{
    GraphML* selectedModelItem = selectedItem->getGraphML();
    if(selectedModelItem->isEdge()){
        return false;
    }
    Node* selectedModelNode = (Node*) selectedModelItem;

    foreach(int ID, selectedIDs){
        GraphMLItem* item = getGraphMLItemFromID(ID);
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


    QList<int> currentlySelectedIDs;
    currentlySelectedIDs.append(selectedIDs);

    while(!currentlySelectedIDs.isEmpty()){
        int ID = currentlySelectedIDs.takeFirst();
        GraphMLItem* item = getGraphMLItemFromID(ID);
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

EntityItem *NodeView::getSharedEntityItemParent(EntityItem *src, EntityItem *dst)
{
    if(controller){
        int ID = controller->getSharedParent(src->getID(), dst->getID());

        EntityItem* node = getEntityItemFromID(ID);
        if(node){
            return node;
        }
    }

    qCritical() << "NO NODE";
    return 0;
}

EntityItem *NodeView::getEntityItemFromNode(Node *node)
{
    return getEntityItemFromGraphMLItem(getGraphMLItemFromGraphML(node));
}

EntityItem *NodeView::getEntityItemFromID(int ID)
{
    EntityItem* node = 0;
    GraphMLItem* g = getGraphMLItemFromID(ID);
    if(g && g->isEntityItem()){
        node = (EntityItem*) g;
    }
    return node;
}

GraphMLItem *NodeView::getGraphMLItemFromID(int ID)
{
    if(guiItems.contains(ID)){
        return guiItems[ID];
    }
    return 0;
}

NodeItem *NodeView::getNodeItemFromID(int ID)
{
    GraphMLItem* item = getGraphMLItemFromID(ID);
    if(item && item->isNodeItem()){
        return (NodeItem*)item;
    }
}

GraphMLItem *NodeView::getGraphMLItemFromScreenPos(QPoint pos)
{
    QPointF scenePos = mapToScene(pos);

    foreach(QGraphicsItem* item, scene()->items(scenePos)){
        GraphMLItem* graphmlItem =  dynamic_cast<GraphMLItem*>(item);
        if(graphmlItem){
            return graphmlItem;
        }
    }
    return 0;
}



GraphMLItem *NodeView::getGraphMLItemFromGraphML(GraphML *item)
{
    if(item){
        return getGraphMLItemFromID(item->getID());
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
                return;
            }
        }
    }else if(viewState == VS_PAN || viewState == VS_PANNING){

        if(viewState == VS_PAN){
            //Check to see if the item under the mouse is selected. if it isn't select it.
            GraphMLItem* item = getGraphMLItemFromScreenPos(event->pos());

            if(item && item->isNodeItem() && !item->isSelected()){
                if(!(event->modifiers() & Qt::ControlModifier)){
                    clearSelection();
                }
                appendToSelection(item);
            }
        }
        if(selectedIDs.length() > 0){
            setState(VS_SELECTED);
        }else{
            setState(VS_NONE);
        }

        //Check panning state.
        QLineF distance(panningOrigin, event->pos());
        if(distance.length() < 5){
            wasPanning = false;
            showToolbar(event->pos());
        }

        return;

    }else if(viewState == VS_CONNECT || viewState == VS_CONNECTING){
        if(event->button() == Qt::LeftButton){
            //Check for item.
            GraphMLItem* item = getGraphMLItemFromScreenPos(event->pos());
            if(item){
                //Attempt to connect.
                constructEdge(getSelectedNodeID(), item->getID());
            }
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

    if(MINIMAP_EVENT){
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    if(viewState == VS_RUBBERBANDING){
        rubberBand->setGeometry(QRect(rubberBandOrigin, event->pos()).normalized());
        return;
    }else if(viewState == VS_CONNECT || viewState == VS_CONNECTING){

        if(viewState == VS_CONNECT){
            setState(VS_CONNECTING);
        }
        EntityItem* item = getSelectedEntityItem();
        if(item){

            QPointF lineStart = item->scenePos();
            lineStart += item->minimumRect().center();

            QPointF lineEnd = mapToScene(event->pos());
            QLineF line(lineStart, lineEnd);

            if (showConnectLine) {
                if(!connectLine){
                    connectLine = scene()->addLine(line);
                }
                connectLine->setLine(line);
                connectLine->setZValue(100);
            }
        }

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
    // TODO: Need to catch the case where the menu is closed
    // when MEDEA window steals the focus
    // need this in case there is an opened lock menu
    if (prevLockMenuOpened && prevLockMenuOpened->isVisible()) {
        prevLockMenuOpened->close();
    }

    Qt::MouseButton buttonPressed = event->button();


    switch(buttonPressed){
    case Qt::LeftButton:{
        QPointF scenePos = mapToScene(event->pos());
        GraphMLItem* itemUnderMouse = getGraphMLItemFromScreenPos(event->pos());

        switch(viewState){
        case VS_NONE:
            //Handle as Selected.
        case VS_SELECTED:
            //If we are currently in the None/Selected states and there is no item under the mouse/.
            if(!itemUnderMouse){
                //If Left Click on no item, clear selection.
                clearSelection();
                return;
            }
            break;
        case VS_RUBBERBAND:
            setState(VS_RUBBERBANDING);
            rubberBandOrigin = event->pos();
            //Move rubberband to the position on the screen.
            rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
            rubberBand->setVisible(true);
            return;

        }
        break;
    }
    case Qt::RightButton:{
        switch(viewState){
        case VS_NONE:
            //Handle as Selected.
        case VS_SELECTED:
            setState(VS_PAN);
            panningOrigin = event->pos();
            panningSceneOrigin = mapToScene(panningOrigin);
            return;
        }
        break;
    }
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

    GraphMLItem* item = getGraphMLItemFromScreenPos(event->pos());

    if(!item){
        if(event->button() == Qt::LeftButton){
            fitToScreen();
        }
    }else{
        QGraphicsView::mouseDoubleClickEvent(event);
    }
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
                if (!editingEntityItemLabel) {
                    selectAll();
                }
            }
        }

        if (event->key() == Qt::Key_Escape){
            setState(VS_NONE);
            clearSelection();
        }
        if(event->key() == Qt::Key_F2){
            if(viewState == VS_NONE || viewState == VS_SELECTED){
                EntityItem* EntityItem = getSelectedEntityItem();
                if(EntityItem){
                    EntityItem->setNewLabel();
                }
            }
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

    if(!(alignment == VERTICAL || alignment == HORIZONTAL)){
        return;
    }


    int itemCount=0;
    qreal averageY=0;
    qreal averageX=0;
    GraphMLItem* sharedParent = 0;

    foreach(int ID, selectedIDs){
        NodeItem* nodeItem = getNodeItemFromID(ID);
        if(nodeItem){
            if(!sharedParent){
                sharedParent = nodeItem->getParent();
            }else if(sharedParent != nodeItem->getParent()){
                view_displayNotification("Cannot align entities which aren't contained by the same parent.");
                return;
            }
            averageX += nodeItem->getMinimumRectCenterPos().x();
            averageY += nodeItem->getMinimumRectCenterPos().y();
            itemCount++;
        }
    }

    triggerAction("Aligning Selection");
    averageX /= itemCount;
    averageY /= itemCount;

    QPointF center;


    //NodeItems have Grids.
    if(sharedParent && sharedParent->isNodeItem()){
        //Find closest Grid Line
        NodeItem* parent = (NodeItem*)sharedParent;
        center = parent->getClosestGridPoint(QPointF(averageX, averageY));
    }

    foreach(int ID, selectedIDs){
        NodeItem* nodeItem = getNodeItemFromID(ID);
        if(nodeItem){
            QPointF pos = nodeItem->getMinimumRectCenterPos();

            if(alignment == VERTICAL){
                pos.setX(center.x());
            }
            if(alignment == HORIZONTAL){
                pos.setY(center.y());
            }

            nodeItem->setMinimumRectCenterPos(pos);
            nodeItem->updatePositionInModel();
        }
    }
}





void NodeView::setEnabled(bool enabled)
{
    //HIDE STUFF
    QGraphicsView::setEnabled(enabled);
}


void NodeView::showMessage(MESSAGE_TYPE type, QString title, QString message, int ID, bool centralizeItem)
{
    GraphMLItem* item = getGraphMLItemFromID(ID);
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
            QList<int> duplicateList = selectedIDs;
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
    int ID = getSelectedNodeID();

    if (ID == -1) {
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
    GraphMLItem* graphMLItem = getSelectedGraphMLItem();
    if(graphMLItem){
        clearSelection();

        foreach(GraphMLItem* child, graphMLItem->getChildren()){
            if(child->isVisible()){
                appendToSelection(child, false);
            }
        }
        // update menu and toolbar actions
        updateActionsEnabledStates();
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


/**
 * @brief NodeView::appendToSelection
 * @param item
 * @param updateActions
 */
void NodeView::appendToSelection(GraphMLItem *item, bool updateActions)
{
    if (isItemsAncestorSelected(item)){
        return;
    }

    //Unset Items Descendant Items.
    unsetItemsDescendants(item);


    //Set this item as Selected.
    setGraphMLItemSelected(item, true);

    // update enabled states of menu and toolbar actions
    if (updateActions) {
        updateActionsEnabledStates();
    }
}

/**
 * @brief NodeView::removeFromSelection
 * @param item
 */
void NodeView::removeFromSelection(GraphMLItem *item)
{
    //Set this item as Selected.
    setGraphMLItemSelected(item, false);

    // added this here because actions weren't being enabled/disabled
    // correctly when selecting/deselting items using the CTRL key
    updateActionsEnabledStates();
}

void NodeView::moveSelection(QPointF delta)
{
    if(viewState == VS_MOVING){
        bool canReduceX = true;
        bool canReduceY = true;

        foreach(int ID, selectedIDs){
            GraphMLItem* graphMLItem = getGraphMLItemFromID(ID);
            GraphML* graphml = graphMLItem->getGraphML();

            if(graphml && graphml->isNode()){
                EntityItem* entityItem = (EntityItem*) graphMLItem;
                QPointF resultingPosition = entityItem->pos() + delta;



                qreal minX = 0;
                qreal minY = 0;
                if(entityItem->getParentEntityItem()){
                    QRectF gridRect = entityItem->getParentEntityItem()->gridRect();
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
            if (graphMLItem && graphMLItem->isEntityItem()) {
                EntityItem* item = (EntityItem*) graphMLItem;
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

        foreach(int ID, selectedIDs){
            GraphMLItem* graphMLItem = getGraphMLItemFromID(ID);
            if(graphMLItem && graphMLItem->isNodeItem()){
                NodeItem* nodeItem = (NodeItem*) graphMLItem;
                if(nodeItem->getParentNodeItem()){
                    nodeItem->getParentNodeItem()->setDrawGrid(true);
                }

                nodeItem->adjustPos(delta);
            }
        }
    }
}

void NodeView::resizeSelection(int ID, QSizeF delta)
{   
    //Cannot resize from any state except None and Resizing.
    if(viewState == VS_RESIZING){
        GraphMLItem* graphMLItem = getGraphMLItemFromID(ID);
        if(graphMLItem && graphMLItem->isNodeItem()){
            ((NodeItem*)graphMLItem)->adjustSize(delta);
        }
    }
}

void NodeView::moveFinished()
{
    foreach(int ID, selectedIDs){
        GraphMLItem* graphMLItem = getGraphMLItemFromID(ID);
        if(graphMLItem && graphMLItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;

            if(nodeItem->getParentNodeItem()){
                nodeItem->getParentNodeItem()->setDrawGrid(false);
                nodeItem->getParentNodeItem()->updateSizeInModel();
            }

            nodeItem->snapToGrid();
        }
    }
    setState(VS_SELECTED);
}

void NodeView::resizeFinished(int ID)
{

    GraphMLItem* currentItem = getGraphMLItemFromID(ID);
    if(currentItem && currentItem->isNodeItem()){
        NodeItem* nodeItem = (NodeItem*) currentItem;
        nodeItem->updateSizeInModel();
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
    while (!selectedIDs.isEmpty()){
        int currentID = selectedIDs.takeFirst();
        GraphMLItem* currentItem = getGraphMLItemFromID(currentID);
        if (currentItem) {
            currentItem->setSelected(false);
        }
    }

    if (updateTable) {
        setAttributeModel(0,false);
    }


    // update menu and toolbar actions
    updateActionsEnabledStates();

    // this stops unnecessary disabling of docks/dock buttons
    // if the call came from a painted node item, just clear the selection
    GraphMLItem* senderItem = dynamic_cast<GraphMLItem*>(QObject::sender());
    if (senderItem) {
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

    ModelItem* modelItem = getModelItem();

    if(!modelItem){
        return;
    }

    QRectF selectionRectangle(fromScenePoint, toScenePoint);

    QList<GraphMLItem*> items;
    items << modelItem;

    while(items.size() > 0){
        GraphMLItem* currentItem = items.takeFirst();

        bool intersects = currentItem->intersectsRectangle(selectionRectangle) && currentItem->isVisible();

        //qCritical() << currentItem->getGraphML();
        if(intersects){
            appendToSelection(currentItem);
        }else{
            if(currentItem->isNodeItem()){
                foreach(EdgeItem* edge, ((NodeItem*)currentItem)->getEdges()){
                    items << edge;
                }
            }
            foreach(GraphMLItem* child, currentItem->getChildren()){
                items << child;
            }
        }
    }
    // Update Actions
    updateActionsEnabledStates();
}


void NodeView::constructGUIItem(GraphML *item){
    //Check if we should construct this item.
    if(isSubView() && controller){
        int ID = item->getID();
        if(!controller->areIDsInSameBranch(centralizedItemID, ID)){
            return;
        }
    }

    if(item->isNode()){
        view_ConstructNodeGUI((Node*)item);
    }else if(item->isEdge()){
        view_ConstructEdgeGUI((Edge*)item);
    }else{
        qCritical() << "Unknown Type";
    }
}


void NodeView::destructGUIItem(int ID, GraphML::KIND kind)
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
        EntityItem* EntityItem = getEntityItemFromNode(node);
        if(EntityItem->getNodeKind() == "ManagementComponent"){
            EntityItem->setHidden(!show);
        }
    }

    EntityItem* assemblyNI = getEntityItemFromNode(assemblyDefinition);
    if(assemblyNI){
        assemblyNI->sortChildren();
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
        EntityItem* EntityItem = getEntityItemFromNode(node);
        if(EntityItem->getNodeKind() == "HardwareNode"){
            EntityItem->setHidden(!show);
        }
    }

    EntityItem* hardwareNI = getEntityItemFromNode(hardwareDefinition);
    if(hardwareNI){
        hardwareNI->sortChildren();
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




void NodeView::setConnectMode(bool on)
{
    if(on){
        EntityItem* srcNode = getSelectedEntityItem();
        if(srcNode){
            foreach(NodeItem* entityItem, getConnectableNodeItems(srcNode->getID())){
                highlightedIDs.append(entityItem->getID());
                entityItem->setHighlighted(true);
            }
        }
    }else{
        while(!highlightedIDs.isEmpty()){
            int ID = highlightedIDs.takeFirst();
            EntityItem* EntityItem = getEntityItemFromID(ID);
            if(EntityItem){
                EntityItem->setHighlighted(false);
            }
        }

        if(connectLine){
            delete connectLine;
            connectLine = 0;
            update();
        }

        showConnectLine = true;
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
void NodeView::fitToScreen(QList<GraphMLItem *> itemsToCenter, double padding, bool addToMap)
{

    if (itemsToCenter.isEmpty() && getModelItem()) {
        ModelItem* modelItem = getModelItem();
        if(modelItem->getVisibleAspects().isEmpty()){
            itemsToCenter.append(getModelItem());
        }else{
            itemsToCenter.append(getModelItem()->getChildren());
        }
    }

    QRectF visibleItemsRect;

    foreach (GraphMLItem* nodeItem, itemsToCenter) {
        if (nodeItem->isVisible()) {
            visibleItemsRect = visibleItemsRect.united(nodeItem->sceneBoundingRect());
        }
    }

    centerRect(visibleItemsRect, padding, addToMap);
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
    foreach (EntityItem* EntityItem, getEntityItemsList()) {
        if (EntityItem) {
            EntityItem->resetSize();
        }
    }

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

