#include "nodeview.h"
#include "../Controller/controller.h"
#include "Toolbar/toolbarwidget.h"
#include "Dock/docktogglebutton.h"
#include "../Widgets/medeasubwindow.h"
#include "../Widgets/medeawindow.h"
#include "GraphicsItems/aspectitem.h"
#include "GraphicsItems/noguiitem.h"
#include <limits>

#include "nodeviewitem.h"
#include "SceneItems/nodeitemnew.h"
#include "SceneItems/edgeitemnew.h"
#include "SceneItems/defaultnodeitem.h"
#include "SceneItems/aspectitemnew.h"

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

#include "SceneItems/nodeitemnew.h"

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR

#define ARROW_PAN_DISTANCE 1
#define VIEW_PADDING 1.1
#define CENTER_ON_PADDING 1 / 0.25

#define MAX_ZOOM_RATIO 200
#define MIN_ZOOM_RATIO 3

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
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    controller = 0;
    wasPanning = false;
    connectLine = 0;

    currentActiveSelectedID = -1;
    constructedFromImport = false;
    toolbarJustClosed = false;
    editingEntityItemLabel = false;
    managementComponentVisible = false;
    toolbarDockConstruction = false;
    importFromJenkins = false;
    hardwareDockOpen = false;
    showConnectLine = true;
    showSearchSuggestions = false;
    aspectVisible = VA_NONE;

    IS_SUB_VIEW = subView;

    setSceneRect(QRectF(-10000,-10000,20000,20000));

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
    SELECT_AFTER_CONSTRUCTION = false;
    CONTROL_DOWN = false;
    SHIFT_DOWN = false;
    IS_DESTRUCTING = false;

    pasting = false;


    viewState = VS_NONE;

    eventFromEdgeItem = false;


    visibleViewRect = viewport()->rect();

    MINIMAP_EVENT = false;

    setScene(new QGraphicsScene(this));

    //Set QT Options for this QGraphicsView
    setDragMode(NoDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    setAcceptDrops(true);

    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);


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
    nonDrawnNodeClasses << NC_DEPLOYMENT_DEFINITIONS << NC_WORKER_DEFINITIONS;


    // construct toolbar widget
    toolbar = new ToolbarWidget(this);
    if (isMainView()) {
        connect(this, SIGNAL(view_updateMenuActionEnabled(QString,bool)), toolbar, SLOT(updateActionEnabledState(QString,bool)));
        //connect(this, SIGNAL(view_themeChanged(VIEW_THEME)), toolbar, SLOT(setupTheme(VIEW_THEME)));
    }

    comboBox = new QComboBox(this);
    comboBox->setVisible(false);
    comboBox->setFixedHeight(0);
    if(isMainView()){
        connect(comboBox, SIGNAL(activated(QString)), this, SLOT(dropDownChangedValue(QString)));
    }


    // initialise the view's center point
    centerPoint = getVisibleRect().center();

    currentMapKey = -1;
    initialRect = 0;
    viewMovedBackForward = false;

    numberOfNotifications = 1;
    notificationNumber = 0;


    //Set Initial zoom.
    currentZoom = transform().m11();

    if(isSubView()){
        themeChanged();
    }
}


void NodeView::setController(NewController *c)
{
    controller = c;
    if(controller){
        connect(controller, SIGNAL(destroyed(QObject*)), this, SLOT(controllerDestroyed()));
    }
}

void NodeView::disconnectController()
{
    controller = 0;
    destroySubViews();
    emit view_ModelDisconnected();
}

VIEW_STATE NodeView::getViewState()
{
    return viewState;
}

qreal NodeView::getCurrentZoom()
{
    return currentZoom;
}

bool NodeView::hasModel()
{
    return controller;
}

bool NodeView::projectRequiresSaving()
{
    if(controller){
        return controller->projectRequiresSaving();
    }
    return false;
}

QString NodeView::getProjectAsGraphML()
{
    QString data;
    if(viewMutex.tryLock()){
        if(controller){
            data = controller->getProjectAsGraphML();
        }
        viewMutex.unlock();
    }
    return data;
}

QString NodeView::getSelectionAsGraphMLSnippet()
{
    QString data;
    if(viewMutex.tryLock()){
        if(controller){
            data = controller->getSelectionAsGraphMLSnippet(selectedIDs);
        }
        viewMutex.unlock();
    }
    return data;
}

QString NodeView::getProjectFileName()
{
    if(controller){
        return controller->getProjectFileName();
    }
    return "";
}

QString NodeView::getImportableSnippetKind()
{
    GraphMLItem* selectedItem = getSelectedGraphMLItem();

    if (selectedItem){
        return selectedItem->getNodeKind();
    }
    return "";
}

void NodeView::scrollContentsBy(int, int)
{

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


/**
 * @brief NodeView::enforceEntityItemVisible
 * Ensure that the item with the provided ID's aspect is turned on and that its parent is expanded.
 * @param ID
 */
void NodeView::enforceEntityItemVisible(int ID)
{
    // TODO - This turns on the aspect and expands the parent item correctly
    // However, when used in selectAndCenter for when a SearchItem is clicked,
    // the item isn't always centered correctly
    EntityItem* entityItem = getEntityItemFromID(ID);
    if (entityItem) {
        enforceItemAspectOn(ID);
        entityItem->forceExpandParentItem();
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


void NodeView::setVisible(bool visible)
{
    QGraphicsView::setVisible(true);
    //Hide only the viewport.
    viewport()->setVisible(visible);
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


void NodeView::setApplicationDirectory(QString appDir)
{
    applicationDirectory = appDir;
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
    }
}

QImage NodeView::renderScreenshot(bool currentViewPort, int quality)
{
    if(quality <=0){
        quality = 1;
    }else if(quality >= 10){
        quality = 10;
    }
    //Store the rect of what we are to print.
    QRectF capturedSceneRect;

    if(currentViewPort){
        capturedSceneRect = getVisibleRect();
    }else{
        if(getModelItem()){
            capturedSceneRect = scene()->itemsBoundingRect();
        }
    }

    QSizeF imageSize = capturedSceneRect.size() * quality;

    QImage image(imageSize.width(), imageSize.height(), QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene()->render(&painter, image.rect(), capturedSceneRect);
    painter.end();
    return image;
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

    if (isSubView()) {
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
    resetTransform();
    scale(newScale, newScale);

    // center the view on rect's original center
    centerViewOn(rectCenter + sceneOffset);
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
    //qWarning() << "NodeView::getModelScenePos - There is no model item.";
    return QPointF();
}


/**
 * @brief NodeView::adjustModelPosition
 * @param delta
 */
void NodeView::adjustModelPosition(QPoint delta)
{
    //Scale by zoom!
    QPointF floatDelta(delta.x(), delta.y());
    floatDelta /= getCurrentZoom();
    adjustModelPosition(floatDelta);
}

void NodeView::adjustModelPosition(QPointF delta)
{
    translate(delta.x(),delta.y());
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

void NodeView::setActiveSelectionItem(int ID)
{
    if(selectedIDs.contains(ID)){
        if(currentActiveSelectedID){
            GraphMLItem* prev = getGraphMLItemFromID(currentActiveSelectedID);
            if(prev){
                prev->setActiveSelected(false);
            }
        }
        currentActiveSelectedID = ID;

        if(currentActiveSelectedID){
            GraphMLItem* prev = getGraphMLItemFromID(currentActiveSelectedID);
            if(prev){
                setAttributeModel(prev);
                prev->setActiveSelected(true);
            }
        }
    }else{
        currentActiveSelectedID = -1;
        setAttributeModel();
    }
}

void NodeView::setNextActiveSelectionItem(bool previous)
{
    int modifier = 1;
    if(previous){
        modifier *= -1;
    }

    int position = selectedIDs.indexOf(currentActiveSelectedID);
    if(position == -1){
        position = 0;
        modifier = 0;
    }

    int max = selectedIDs.count();

    position += modifier;

    if(position >= max){
        position = 0;
    }
    if(position < 0){
        position = max - 1;
    }
    int nextItem = -1;
    if(position >= 0 && position < selectedIDs.count()){
        nextItem = selectedIDs[position];
    }
    setActiveSelectionItem(nextItem);
}

void NodeView::translate(qreal dx, qreal dy)
{
    QGraphicsView::translate(dx, dy);
    viewportTranslated();
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


/**
 * @brief NodeView::getSharedParentNodeItem
 * @param graphMLItems
 * @return
 */
NodeItem* NodeView::getSharedParentNodeItem(QList<GraphMLItem*> graphMLItems)
{
    QList<NodeItem*> nodeItems;
    foreach (GraphMLItem* item, graphMLItems) {
        if (item->isNodeItem()) {
            nodeItems.append((NodeItem*)item);
        }
    }

    if (nodeItems.isEmpty() || nodeItems.count() == 1) {
        return 0;
    }

    // check if all the provided items have a shared parent item
    NodeItem* prevParentItem = 0;
    foreach (NodeItem* nodeItem, nodeItems) {
        NodeItem* parentItem = nodeItem->getParentNodeItem();
        if (prevParentItem && (prevParentItem != parentItem)) {
            return 0;
        }
        prevParentItem = parentItem;
    }
    return prevParentItem;
}

void NodeView::updateDeploymentWarnings(int nodeID)
{
    bool showHardwareWarning = hardwareDockOpen;

    QList<int> highlightedIDs;
    QList<int> toUpdateIDs = getSelectedNodeIDs();
    if(nodeID != -1){
        toUpdateIDs.append(nodeID);
    }

    while(!toUpdateIDs.isEmpty()){
        int ID = toUpdateIDs.takeFirst();
        bool isSelected = selectedIDs.contains(ID);

        EntityItem* node = getEntityItemFromID(ID);
        EntityItem* parent = 0;

        if(node){
            GraphMLItem* parentG = node->getParent();

            int deployedID = getDeployedHardwareID(node);

            if(parentG && parentG->isEntityItem()){
                parent = (EntityItem*) parentG;

                int parentDeployedID = getDeployedHardwareID(parent);
                if((parentDeployedID != -1 && deployedID != -1) && (parentDeployedID != deployedID)){
                    parent->showDeploymentWarning(true);
                }
            }

            bool showNotification = false;

            foreach(EntityItem* child, node->getChildEntityItems()){
                int childID = child->getID();
                int childDeployedID = getDeployedHardwareID(child);
                bool highlight = false;
                if((childDeployedID != -1 && deployedID != -1) && (childDeployedID != deployedID)){
                    showNotification = true;
                    if(showHardwareWarning && isSelected){
                        highlight = true;
                    }
                }
                if(highlight){
                    highlightedIDs.append(childID);
                    child->setHardwareHighlighting(true);
                }
            }
            node->showDeploymentWarning(showNotification);
        }
    }

    QList<int> currentlyHighlighted = hardwareHighlightedIDs;

    while(!currentlyHighlighted.isEmpty()){
        int ID = currentlyHighlighted.takeFirst();
        EntityItem* node = getEntityItemFromID(ID);

        if(node){
            if(!highlightedIDs.contains(ID)){
                node->setHardwareHighlighting(false);
            }
        }
    }

    hardwareHighlightedIDs = highlightedIDs;
}

int NodeView::getDeployedHardwareID(EntityItem *item)
{
    int deployedID = -1;

    if(item){
        NodeAdapter* nA = item->getNodeAdapter();
        if(nA){
            //Get the Deployed Hardware Entity.
            foreach(int edgeID, nA->getEdgeIDs(Edge::EC_DEPLOYMENT)){
                deployedID = nA->getChildEdgeDstID(edgeID);
                break;
            }
        }
    }
    return deployedID;
}


void NodeView::dragEnterEvent(QDragEnterEvent *event)
{
    //Ignore the event so that MEDEA window will handle it.
    event->ignore();
}

void NodeView::dropEvent(QDropEvent *event)
{
    //Ignore the event so that MEDEA window will handle it.
    event->ignore();
}

bool NodeView::focusNextPrevChild(bool)
{
    return false;
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
    if (hasFocus() && editingEntityItemLabel) {
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

QList<QPair<QString, QString> >  NodeView::getFunctionsList()
{
    QList<QPair<QString, QString> > functionList;
    if(controller){
        foreach(int ID, controller->getFunctionIDList()){
            QPair<QString, QString> functionPair;
            functionPair.first = getData(ID, "worker");
            functionPair.second = getData(ID, "operation");
            if(functionPair.first.isEmpty() || functionPair.second.isEmpty()){
                continue;
            }
            functionList.append(functionPair);
        }
    }
    return functionList;
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
 * @param viewAspects
 * @param entityKinds
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
        foreach(GraphMLItem* item, getEntityItemsList()){
            itemsToSearch += item;
        }
    } else if (kind == GraphMLItem::NODE_EDGE) {
        foreach(GraphMLItem* item, getEdgeItemsList()){
            itemsToSearch += item;
        }
    }

    QRegExp regex(searchString, Qt::CaseInsensitive, QRegExp::Wildcard);
    bool allAspects = viewAspects.isEmpty();
    bool allKinds = entityKinds.isEmpty();

    foreach (GraphMLItem* item, itemsToSearch) {

        if (kind == GraphMLItem::ENTITY_ITEM) {

            EntityItem* entityItem = (EntityItem*)item;
            QString itemAspect = GET_ASPECT_NAME(entityItem->getViewAspect());

            // check if item's aspect is in the view aspects list
            if (!allAspects && !viewAspects.contains(itemAspect)) {
                continue;
            }

            // check if item's kind is in the entity kinds list
            if (!allKinds && !entityKinds.contains(item->getNodeKind())) {
                continue;
            }
        }

        EntityAdapter* gml = item->getEntityAdapter();
        if (!gml) {
            continue;
        }

        // the data table doesn't contain an id key - deal with it separately
        // don't show any id suggestions and only add item with an exact id match
        if (!showSearchSuggestions && dataKeys.contains("id")) {
            if (QString::number(item->getID()) == searchString.remove("*")) {
                if (!matchedItems.contains(item)) {
                    matchedItems.append(item);
                    if (dataKeys.size() == 1) {
                        break;
                    }
                }
            }
        }

        // if searchString matches at least one of the values of the provided
        // data keys for the current graphml item, append the item to the list
        foreach (QString key, dataKeys) {
            QString dataVal = gml->getDataValue(key).toString();
            if (dataVal.isEmpty()) {
                continue;
            }
            if (regex.exactMatch(dataVal)) {
                if (!matchedItems.contains(item)) {
                    matchedItems.append(item);
                }
                if (!matchedDataValues.contains(dataVal)) {
                    matchedDataValues.append(dataVal);
                }
                if (!showSearchSuggestions) {
                    break;
                }
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

QList<int> NodeView::getConnectableNodes(QList<int> IDs)
{
    bool firstComponent = true;
    QList<int> validComponents;

    foreach(int ID, IDs){
        QList<int> validIDs = controller->getConnectableNodes(ID);

        if(firstComponent){
            firstComponent = false;
            validComponents = validIDs;
        }else{
            QList<int> tempList = validComponents;
            validComponents.clear();
            while(!tempList.isEmpty()){
                int currentID = tempList.takeFirst();
                if(validIDs.contains(currentID)){
                    validComponents.append(currentID);
                }
            }
            if(validComponents.isEmpty()){
                break;
            }
        }
    }

    return validComponents;
}

QList<NodeItem *> NodeView::getConnectableNodeItems(int ID)
{
    QList<NodeItem*> nodeItems;
    QList<int> IDs = controller->getConnectableNodes(ID);
    foreach(int cID, IDs){
        NodeItem* entityItem = getEntityItemFromID(cID);

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
    QList<NodeItem*> nodes;

    foreach(int ID, getConnectableNodes(IDs)){
        NodeItem* item = getNodeItemFromID(ID);
        if(item){
            nodes.append(item);
        }
    }

    return nodes;
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
QList<EntityItem*> NodeView::getHardwareList()
{
    QList<EntityItem*> hardwares;
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
 * @param nodeKindToCenter - -1 = selected node
 *                           -2 = selected node's definition
 *                           -3 = selected node's implementation
 */
void NodeView::constructNewView(int viewKind)
{
    if(!controller){
        return;
    }
    int nodeID = getSelectedID();

    switch (viewKind) {
    case -1:
        break;
    case -2:
        nodeID = getDefinitionID(nodeID);
        break;
    case -3:
        nodeID = getImplementationID(nodeID);
        break;
    default:
        nodeID = viewKind;
    }


    if (nodeID <= 0) {
        qWarning() << "NodeView::constructNewView - nodeID is null.";
        return;
    }

    GraphMLItem* centeredNode = getGraphMLItemFromID(nodeID);

    if (IS_SUB_VIEW || !centeredNode){
        return;
    }

    MedeaSubWindow* subWindow = new MedeaSubWindow();
    connect(this, SIGNAL(destroyed()), subWindow, SLOT(reject()));

    NodeView* newView = new NodeView(true , subWindow);
    subViews.append(newView);
    newView->setParentNodeView(this);

    subWindow->setNodeView(newView);
    newView->view_LockCenteredGraphML(nodeID);

    controller->connectView(newView);


    QList<GraphMLItem*> constructList;
    GraphMLItem* parent = centeredNode->getParent();

    while(parent){
        constructList.insert(0, parent);
        parent = parent->getParent();
    }

    //Construct Parents first.
    while(!constructList.isEmpty()){
        GraphMLItem* item = constructList.takeFirst();
        newView->constructEntityItem(item->getEntityAdapter());
    }

    constructList.append(centeredNode);

    while(!constructList.isEmpty()){
        GraphMLItem* item = constructList.takeFirst();
        newView->constructEntityItem(item->getEntityAdapter());

        foreach(GraphMLItem* child, item->getChildren()){
            constructList.insert(0, child);
        }

        if(item->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) item;

            foreach(GraphMLItem* edge, nodeItem->getEdges()){
                constructList.append(edge);
            }
        }
    }

    connect(this, SIGNAL(view_ClearSubViewAttributeTable()), newView, SIGNAL(view_ClearSubViewAttributeTable()));
    subWindow->show();
    //Centralize item.
    newView->centralizedItemMoved();
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

void NodeView::resetViewState()
{
    selectedIDs.clear();
    highlightedIDs.clear();
    subViews.clear();
    viewCenterPointStack.clear();
    viewModelPositions.clear();
    viewCenteredRectangles.clear();
    modelPositions.clear();
    centeredRects.clear();
    guiItems.clear();
    noGUIItems.clear();
    viewState = VS_NONE;

    emit view_ProjectNameChanged("");
    emit view_ProjectFileChanged("");
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

void NodeView::viewItemConstructed(ViewItem *viewItem)
{
    if(viewItem->isNode()){
        NodeViewItem* nodeViewItem = (NodeViewItem*)viewItem;


        NodeItemNew* parent = 0;
        int depth = 1;
        int parentID = -2;
        while(depth > 0 && parentID != -1){
            parentID = nodeViewItem->getParentID(depth);
            if(newNodeItems.contains(parentID)){
                parent = (NodeItemNew*)newNodeItems[parentID];
                break;
            }
            depth ++;
        }

        QString nodeKind = nodeViewItem->getData("kind").toString();

        NODE_CLASS nodeClass = nodeViewItem->getNodeClass();


        //Check if we should construct this Node.
        if(nonDrawnNodeClasses.contains(nodeClass) || !nodeViewItem->isInModel()){
            return;
        }

        NodeItemNew* nodeItem =  0;
        if(nodeKind.endsWith("Definitions")){
            VIEW_ASPECT aspect = GET_ASPECT_FROM_KIND(nodeKind);
            nodeItem = new AspectItemNew(nodeViewItem, parent, aspect);
        }else{
            if(parent || (!parent  && nodeKind != "Model")){
                nodeItem = new DefaultNodeItem(nodeViewItem, parent);
            }
        }
        if(nodeItem){

            newNodeItems[viewItem->getID()] = nodeItem;


            connect(nodeItem, SIGNAL(req_adjustSize(NodeViewItem*,QSizeF, RECT_VERTEX)), this, SLOT(nodeItemNew_AdjustSize(NodeViewItem*,QSizeF,RECT_VERTEX)));
            connect(nodeItem, SIGNAL(req_setData(ViewItem*,QString,QVariant)), this, SLOT(nodeItemNew_SetData(ViewItem*,QString,QVariant)));
            connect(nodeItem, SIGNAL(req_hovered(EntityItemNew*,bool)), this, SLOT(entityItemNew_Hovered(EntityItemNew*, bool)));
            connect(nodeItem, SIGNAL(req_expanded(EntityItemNew*,bool)), this, SLOT(entityItemNew_Expand(EntityItemNew*,bool)));
            connect(nodeItem, SIGNAL(req_setSelected(ViewItem*,bool)), this, SLOT(entityItemNew_Select(ViewItem*,bool)));
            connect(nodeItem, SIGNAL(req_clearSelection()), this, SLOT(clearSelection()));
            connect(nodeItem, SIGNAL(req_adjustPos(QPointF)), this, SLOT(moveSelection(QPointF)));
            connect(nodeItem, SIGNAL(req_adjustPosFinished()), this, SLOT(moveFinished()));

            if(!scene()->items().contains(nodeItem)){
               scene()->addItem(nodeItem);
            }//
        }
    }
}



QPointF NodeView::getCenterOfScreenScenePos(QPoint mousePosition)
{
    QPoint centerPos = viewport()->rect().center();
    if(!mousePosition.isNull()){
        centerPos = mousePosition;
    }
    QPointF centerScenePos = mapToScene(centerPos);
    return centerScenePos;
}

void NodeView::canUndo(bool okay)
{
    emit view_updateMenuActionEnabled("undo", okay);
}

void NodeView::canRedo(bool okay)
{
    emit view_updateMenuActionEnabled("redo", okay);
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
                emit view_SetData(ID, dataType, value);
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
        comboBox->setMinimumWidth(width);
        comboBox->move(topLeft);
        comboBox->showPopup();
        comboBox->setVisible(true);
    }
}



/**
 * @brief NodeView::aspectGraphicsChanged
 * This checks if the current viewport is completely contained within one of the dispalyed aspects.
 * If it is, highlight the corresponding aspect's toggle button's label.
 */
void NodeView::viewportTranslated()
{
    if (isSubView() || !getModelItem()) {
        return;
    }

    //Update ViewPortRect if it's changed.
    QRectF visibleRect = getVisibleRect();
    if(currentVisibleRect != visibleRect){
        currentVisibleRect = visibleRect;
        emit view_ViewportRectChanged(currentVisibleRect);
    }

    //Update Zoom if it's changed.
    qreal newZoom = transform().m11();
    if(newZoom != currentZoom){
        currentZoom = newZoom;
        emit view_ZoomChanged(currentZoom);
    }


    VIEW_ASPECT aspectContained = VA_NONE;

    foreach(int aspectID, aspectIDs){
        NodeItem* aspectItem = getNodeItemFromID(aspectID);
        if(aspectItem && aspectItem->isVisible()){
            QRectF sceneRect = aspectItem->sceneBoundingRect();
            if(sceneRect.contains(visibleRect)){
                aspectContained = aspectItem->getViewAspect();
                break;
            }
        }
    }

    if(aspectVisible != aspectContained){
        aspectVisible = aspectContained;
        emit view_highlightAspectButton(aspectVisible);
    }
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

void NodeView::request_ExportSnippet()
{

    QList<GraphMLItem*> selectedItems = getSelectedItems();

    NodeItem* nodeItem = 0;

    foreach(GraphMLItem* item, selectedItems){
        if(item && item->isNodeItem()){
            nodeItem = (NodeItem*)item;
        }
    }

    if (nodeItem){
        GraphMLItem* parentItem = nodeItem->getParent();
        if(parentItem){
            emit view_ExportSnippet(parentItem->getNodeKind());
        }
    }
}

void NodeView::entitySetReadOnly(int ID, bool isReadOnly)
{
    Q_UNUSED(isReadOnly);
    if(selectedIDs.contains(ID)){
        //Only refresh dock if we have this item in selection.
        emit view_RefreshDock();
    }
}


/**
 * @brief NodeView::hardwareDockOpened
 * @param opened
 */
void NodeView::hardwareDockOpened(bool opened)
{
    hardwareDockOpen = opened;
    updateDeploymentWarnings();
}


void NodeView::showQuestion(MESSAGE_TYPE type, QString title, QString message, int ID)
{
    Q_UNUSED(type);
    if(ID != -1){
        GraphMLItem* item = getGraphMLItemFromID(ID);
        if(item){
            centerItem(item);
        }
    }

    QMessageBox msgBox(QMessageBox::Question, title, message, QMessageBox::Yes | QMessageBox::No);

    msgBox.setIconPixmap(Theme::theme()->getImage("Actions", "Help").scaled(50,50));
    int reply = msgBox.exec();
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
            if(!importFromJenkins){
                //Only clear selection from non jenkins imports
                clearSelection();
            }
            emit view_ImportProjects(xmlDataList);
        }
    }
}

void NodeView::openProject(QString fileName, QString fileData)
{
    if(viewMutex.tryLock()){
        constructedFromImport = true;
        emit view_OpenProject(fileName, fileData);
    }
}

void NodeView::saveProject(QString filePath="")
{
    if(viewMutex.tryLock()){
        emit view_SaveProject(filePath);
    }

}


void NodeView::loadJenkinsNodes(QString fileData)
{
    importFromJenkins = true;
    viewDeploymentAspect();
    importProjects(QStringList(fileData));
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

void NodeView::scrollEvent(int delta, QPoint mouseCenter)
{
    if(getModelItem() && (viewState == VS_NONE || viewState ==  VS_SELECTED || viewState == VS_CONNECT || viewState == VS_CONNECTING)){
        QRectF modelRect = scene()->itemsBoundingRect();

        qreal modelSize, viewSize;

        // use the bigger side of the model to check model:viewport ratio
        if (modelRect.width() > modelRect.height()) {
            modelSize = modelRect.width() * getCurrentZoom();
            viewSize = viewport()->rect().width();
        } else {
            modelSize = modelRect.height() * getCurrentZoom();
            viewSize = viewport()->rect().height();
        }

        QPointF previousSceneCenter = getCenterOfScreenScenePos(mouseCenter);
        bool zoomChanged = false;

        if (delta > 0) {
            // zoom in - maximum scale is when the scene is 100 times the size of the view
            if (viewSize * MAX_ZOOM_RATIO > modelSize) {
                scale(ZOOM_SCALE_INCREMENTOR, ZOOM_SCALE_INCREMENTOR);
                zoomChanged = true;
            }
        } else if (delta < 0) {
            // zoom out - minimum scale is when the view is twice the size of the scene
            if (viewSize < modelSize * MIN_ZOOM_RATIO) {
                scale(ZOOM_SCALE_DECREMENTOR, ZOOM_SCALE_DECREMENTOR);
                zoomChanged = true;
            }
        }

        if (zoomChanged) {
            QPointF newSceneCenter = getCenterOfScreenScenePos(mouseCenter);
            QPointF delta = newSceneCenter - previousSceneCenter;
            translate(delta.x(), delta.y());
        }
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
    if(viewState == VS_PANNING || viewState == VS_PAN){
        if(selectedIDs.isEmpty()){
            setState(VS_NONE);
        }else{
            setState(VS_SELECTED);
        }
    }
}

void NodeView::minimapScrolled(int delta)
{
    scrollEvent(delta);
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

    emit view_TriggerAction(action);
    //updateActionsEnabledStates();
}

void NodeView::minimapPan()
{
    if(viewState == VS_NONE || viewState == VS_SELECTED){
        setState(VS_PAN);
    }
}


/**
 * @brief NodeView::toggleAspect
 * @param aspect
 * @param on
 */
void NodeView::toggleAspect(VIEW_ASPECT aspect, bool on)
{
    AspectItem* aspectItem = getAspectItem(aspect);
    if (aspectItem) {
        aspectItem->setVisible(on);
    }

    if (AUTO_CENTER_ASPECTS) {
        fitToScreen();
    }

    // if the selected item's aspect is turned off, remove it from selection
    if (!on) {
        EntityItem* selectedItem = getSelectedEntityItem();
        if (selectedItem) {
            AspectItem* entityAspect = getAspectItem(selectedItem->getViewAspect());
            if (entityAspect && entityAspect == aspectItem) {
                removeFromSelection(selectedItem);
            }
        }
    }

    viewportTranslated();
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
        triggerAction("View: Sorting Selection");
        emit view_updateProgressStatus(-1, "Sorting selected entities");

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
        triggerAction("View: Expanding/Contracting Selection");
        emit view_updateProgressStatus(-1, "Expanding/contracting selected entities");

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
                    emit view_SetData(entity->getID(), "isExpanded", expand);
                }
            }
        }
        update();
        emit view_updateProgressStatus(100);
        actionFinished();

    }

}

void NodeView::setupSoundEffects()
{
    /*
    if(!clickSound){
        clickSound = new QSoundEffect(this);
        clickSound->setSource(QUrl("qrc:/Sounds/click.wav"));
        clickSound->setLoopCount(1);
        clickSound->setVolume(1);

        connect(this, SIGNAL(view_ModelReady()), clickSound, SLOT(play()));
    }
    */
}

void NodeView::entityItemNew_Hovered(EntityItemNew *item, bool hovered)
{
    item->setHovered(hovered);
}

void NodeView::nodeItemNew_SetData(ViewItem *item, QString key, QVariant data)
{
    emit view_SetData(item->getID(), key, data);
}

void NodeView::nodeItemNew_AdjustSize(NodeViewItem *item, QSizeF delta, RECT_VERTEX vertex)
{
    int ID = item->getID();
    NodeItemNew* nodeItem = newNodeItems[ID];
    if(nodeItem){
        qreal deltaX = delta.width();
        qreal deltaY = delta.height();

        qreal deltaW = delta.width();
        qreal deltaH = delta.height();
        if(vertex == RV_TOP || vertex == RV_BOTTOM){
            //Ignore width changes
            deltaW = 0;
            deltaX = 0;
        }else if(vertex == RV_LEFT || vertex == RV_RIGHT){
            deltaH = 0;
            deltaY = 0;
        }

        if(vertex == RV_TOP || vertex == RV_TOPLEFT || vertex == RV_TOPRIGHT){
            //Invert the H
            deltaH *= -1;
        }
        if(vertex == RV_TOPLEFT || vertex == RV_LEFT || vertex == RV_BOTTOMLEFT){
            //Invert the W
            deltaW *= -1;
        }

        if(vertex == RV_BOTTOM || vertex == RV_BOTTOMLEFT || vertex == RV_BOTTOMRIGHT){
            //Ignore the delta Y
            deltaY = 0;
        }
        if(vertex == RV_RIGHT || vertex == RV_BOTTOMRIGHT || vertex == RV_TOPRIGHT){
            //Ignore the delta X
            deltaX = 0;
        }

        if(deltaW == 0){
            deltaX = 0;
        }
        if(deltaH == 0){
            deltaY = 0;
        }

        qreal currentW = nodeItem->getData("width").toReal();
        qreal currentH = nodeItem->getData("height").toReal();
        qreal currentX = nodeItem->getData("x").toReal();
        qreal currentY = nodeItem->getData("y").toReal();

        nodeItem->setData("x", currentX + deltaX);
        nodeItem->setData("y", currentY + deltaY);
        nodeItem->setData("width", currentW + deltaW);
        nodeItem->setData("height", currentH + deltaH);
    }
}

void NodeView::entityItemNew_Select(ViewItem *item, bool select)
{
    GraphMLItem* item2 = getGraphMLItemFromID(item->getID());
    setGraphMLItemSelected(item2, select);
}

void NodeView::entityItemNew_Expand(EntityItemNew *item, bool expand)
{
    item->setExpanded(expand);
}

void NodeView::controllerDestroyed()
{
    disconnectController();
    resetViewState();

    //Clear the toolbar menus
    if(toolbar){
        toolbar->clearToolbarMenus();
    }

}

void NodeView::settingChanged(QString, QString keyName, QVariant value)
{
    bool boolValue = value.toBool();

    if(keyName == LOG_DEBUGGING){
        emit view_EnableDebugLogging(boolValue, applicationDirectory);
    }else if(keyName == SELECT_ON_CREATION){
        selectNodeOnConstruction(boolValue);
    }else if(keyName == AUTO_CENTER_VIEW){
        autoCenterAspects(boolValue);
    }else if(keyName == SELECT_ON_CREATION){
        selectNodeOnConstruction(boolValue);
    }else if(keyName == ZOOM_ANCHOR_ON_MOUSE){
        toggleZoomAnchor(boolValue);
    }else if(keyName == SHOW_MANAGEMENT_COMPONENTS){
        showManagementComponents(boolValue);
    }else if(keyName == SHOW_LOCAL_NODE){
        showLocalNode(boolValue);
    }
}

void NodeView::modelReady()
{
    //Do initializing here!
    if(toolbar){
        toolbar->setupFunctionsList();
    }

    emit view_LoadSettings();

    emit view_ModelSizeChanged();
    emit view_ModelReady();
    emit view_ProjectFileChanged("Untitled Project");
    updateActionsEnabledStates();

}

void NodeView::themeChanged()
{
   QString bgColor =  Theme::theme()->getBackgroundColorHex();
   QString textColor =  Theme::theme()->getTextColorHex();

    setStyleSheet("QGraphicsView {"
                  "background-color:" + bgColor + ";"
                  "border: 0px;}");
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

void NodeView::selectionChanged()
{
    updateActionsEnabledStates(true);
}


/**
 * @brief NodeView::highlightOnHover
 * This highlights the corresponding node item on the canvas that the cursor
 * is currently hovering over in one of the context toolbar menus.
 * @param nodeID
 */
void NodeView::highlightOnHover(int nodeID)
{
    // turn highlight off for previously highlighted item
    if (prevHighlightedFromToolbarID != nodeID) {
        EntityItem* item = getEntityItemFromID(prevHighlightedFromToolbarID);
        if (item) {
            item->setHighlighted(false);
        }
        prevHighlightedFromToolbarID = -1;
    }

    // highlight item with the provided ID
    if (nodeID != -1) {
        EntityItem* entityItem = getEntityItemFromID(nodeID);
        if (entityItem) {
            entityItem->setHighlighted(true);
            prevHighlightedFromToolbarID = nodeID;
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
        enforceEntityItemVisible(item->getID());
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
            clearSelection();
            appendToSelection(definition, true);
            centerItem();
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
            clearSelection();
            appendToSelection(impl, true);
            centerItem();
        }
    }

}

void NodeView::centerInstance(int instanceID)
{
    if(controller){
        GraphMLItem *instance = getEntityItemFromID(instanceID);

        if(instance){
            // make sure the Assembly view aspect is on
            clearSelection();
            appendToSelection(instance, true);
            centerItem();
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
        //Get the current Selected Item.
        item = getSelectedGraphMLItem();
    }

    if (item) {
        // if the selected node is a main container, just use centerItem()
        // we would only ever want to center and zoom into it
        if (item->isAspectItem()) {
            centerItem(item);
        } else {
            enforceEntityItemVisible(item->getID());
            centerRect(item->sceneBoundingRect(), CENTER_ON_PADDING, true);
        }

    } else {
        view_DisplayNotification("Select entity to center on.");
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

        // if parent entityItem is a HardwareCluster, set its viewMode to ALL
        GraphMLItem* parentItem = item->getParent();
        if (parentItem && parentItem->isEntityItem()) {
            EntityItem* parentEntity = (EntityItem*) parentItem;
            if (parentEntity->isHardwareCluster()) {
                parentEntity->updateChildrenViewMode(0);
            }
        }

        // clear the selection, select the item and then center on it
        clearSelection();
        appendToSelection(item);
        //centerOnItem(item);
        centerItem(item);

    } else {
        view_DisplayNotification("Entity no longer exists!");
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

    if (item->getEntityAdapter()->getDataValue("kind") == "Model") {
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
 * @brief NodeView::replicateCountChanged
 * @param x
 */
void NodeView::replicateCountChanged(int x)
{
    foreach (GraphMLItem* item, getSelectedItems()) {
        if (item && item->isEntityItem() && item->getNodeKind() == "ComponentAssembly") {
            emit view_SetData(item->getID(), "replicate_count", x);
        }
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

        NodeItemNew* nvi = newNodeItems[ID];
        if(nvi){
            nvi->setHovered(enter);
        }
        current->setHovered(enter);

        if(enter){
            GraphMLItem* prev = getGraphMLItemFromID(prevHighlightedID);
            if(prev){
                prev->setHovered(false);
            }

            nvi = newNodeItems[prevHighlightedID];
            if(nvi){
                nvi->setHovered(false);
            }

            prevHighlightedID = ID;
        }else{
            prevHighlightedID = -1;
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
    typeKinds << "Attribute" << "Member" << "OutEventPort" << "InEventPort" << "AggregateInstance" << "VectorInstance" << "OutEventPortInstance" << "InEventPortInstance" << "OutEventPortDelegate" << "InEventPortDelegate" << "AttributeImpl" <<"OutEventPortImpl" << "InEventPortImpl" << "ComponentInstance" << "ReturnType";

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

    if(nodeKind == "Vector"){
        returnType.first = "max_size";
    }

    if(nodeKind == "PeriodicEvent"){
        returnType.first = "frequency";
    }
    if(nodeKind == "Model"){
        returnType.first = "middleware";
    }
    if(nodeKind == "Process"){
        returnType.first = "worker";
    }

    if(dropdownKinds.contains(nodeKind)){
        returnType.second = true;
    }

    return returnType;
}

QPair<QString, bool> NodeView::getStatusDataKeyName(GraphMLItem *node)
{
    QPair<QString, bool> returnType;
    returnType.first = "";
    returnType.second = false;

    QString nodeKind = node->getNodeKind();

    if(nodeKind == "ComponentAssembly"){
        returnType.first = "replicate_count";
        returnType.second = true;
    }
    return returnType;
}

bool NodeView::isNodeVisuallyConnectable(NodeAdapter *node)
{
    if(node){
        if(node->isBehaviourAdapter()){
            BehaviourNodeAdapter* bna = (BehaviourNodeAdapter*) node;
            return bna->needsConnection();
        }else{
            QString nodeKind = node->getDataValue("kind").toString();
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


void NodeView::setState(VIEW_STATE state)
{
    //Only let through VS_NONE, VS_SELECTED and VS_PANNING, otherwise, use VS_NONE.
    if(isSubView() && !(state ==  VS_SELECTED || state == VS_PANNING || state == VS_PAN || state == VS_NONE)){
        state = VS_NONE;
    }
    if(!hasModel()){
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

qreal NodeView::getArrowKeyDelta(bool SHIFT, bool neg)
{
    qreal distance = ARROW_PAN_DISTANCE * getCurrentZoom();
    if(neg){
        distance *= -1;
    }
    if(SHIFT){
        distance *= 10;
    }
    return distance;
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
        triggerAction("View: Moving Selection");
        setCursor(Qt::SizeAllCursor);
        break;
    case VS_RESIZING:
        triggerAction("View: Resizing Selection");
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
            emit view_Delete(IDs);
        }
    } else {
        view_DisplayNotification("Select entity(s)/connection(s) to delete.");
    }
}


/**
 * @brief NodeView::updateActionsEnabledStates
 */
void NodeView::updateActionsEnabledStates(bool updateDocks)
{
    if (!controller) {
        return;
    }

    int selectedID = getSelectedNodeID();
    int selectedNodeItemsCount = getSelectedNodeIDs().size();
    int defnID = controller->getDefinition(selectedID);
    int implID = controller->getImplementation(selectedID);
    bool canExport = controller->canExportSnippet(selectedIDs);
    bool canImport = controller->canImportSnippet(selectedIDs);
    bool canAlign = getSharedParentNodeItem(getSelectedItems()) != 0;

    emit view_updateMenuActionEnabled("cut", controller->canCut(selectedIDs));
    emit view_updateMenuActionEnabled("copy", controller->canCopy(selectedIDs));
    emit view_updateMenuActionEnabled("replicate", controller->canReplicate(selectedIDs));
    emit view_updateMenuActionEnabled("paste", controller->canPaste(selectedIDs));
    emit view_updateMenuActionEnabled("delete", controller->canDelete(selectedIDs));
    emit view_updateMenuActionEnabled("undo", controller->canUndo());
    emit view_updateMenuActionEnabled("redo", controller->canRedo());
    emit view_updateMenuActionEnabled("localDeployment", controller->canLocalDeploy());
    emit view_updateMenuActionEnabled("getCPP", controller->canGetCPP(selectedIDs));
    emit view_updateMenuActionEnabled("setReadOnly", controller->canSetReadOnly(selectedIDs));
    emit view_updateMenuActionEnabled("unsetReadOnly", controller->canUnsetReadOnly(selectedIDs));
    emit view_updateMenuActionEnabled("subView",  selectedID != -1);

    emit view_updateMenuActionEnabled("sort", !getSelectedNodeIDs().isEmpty());
    emit view_updateMenuActionEnabled("wiki", getSelectedNodeIDs().size() == 1);

    emit view_updateMenuActionEnabled("sort", selectedNodeItemsCount != 0);
    emit view_updateMenuActionEnabled("wiki", selectedNodeItemsCount == 1);

    emit view_updateMenuActionEnabled("singleSelection", selectedID != -1);
    emit view_updateMenuActionEnabled("multipleSelection", selectedNodeItemsCount != 0);

    // update other menu actions and toolbar buttons
    emit view_updateMenuActionEnabled("definition", defnID != -1);
    emit view_updateMenuActionEnabled("implementation", implID != -1);
    emit view_updateMenuActionEnabled("exportSnippet", canExport);
    emit view_updateMenuActionEnabled("importSnippet", canImport);

    emit view_updateMenuActionEnabled("align", canAlign);

    if (updateDocks) {
        emit view_nodeSelected();
    }
}


/**
 * @brief NodeView::showToolbar
 * This is called when there is a mouse right-click event.
 * It shows/hides the context toolbar depending on where the right click happened.
 * @param position
 */
void NodeView::showToolbar(QPoint position)
{
    // for now, don't show the toolbar for subviews
    if (isSubView()) {
        return;
    }

    //If we have panned, we shouldn't show the toolbar.
    if (wasPanning) {
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
            toolbar->show();
            toolbar->setFocus();
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
    if (viewMutex.tryLock()) {
        NodeItem* selectedItem = getSelectedNodeItem();
        if (!selectedItem) {
            return;
        }
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
            triggerAction("Dock/Toolbar: Destructing/Constructing Multiple Edges");
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
    if (viewMutex.tryLock()) {

        NodeItem* nodeItem = getNodeItemFromID(parentID);

        if (nodeItem) {

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

        } else {
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
    if (!controller) {
        return;
    }

    int selectedID = getSelectedID();
    QList<GraphMLItem*> connectedItems;

    foreach (int cnID, controller->getConnectedNodes(selectedID)) {
        GraphMLItem* item = getGraphMLItemFromID(cnID);
        if (item) {
            // need to make sure that the aspect for the item is turned on before selecting it
            enforceEntityItemVisible(cnID);
            appendToSelection(item);
            connectedItems.append(item);
        }
    }

    if (!connectedItems.isEmpty()) {
        // add the selected node to the list of items to center
        connectedItems.append(getGraphMLItemFromID(selectedID));
        fitToScreen(connectedItems);
    }
}




void NodeView::setGraphMLItemSelected(GraphMLItem *item, bool setSelected)
{
    if(!item){
        return;
    }

    int ID = item->getID();
    EntityAdapter* itemAdapter = item->getEntityAdapter();
    NodeAdapter* nodeAdapter = 0;
    if(itemAdapter && itemAdapter->isNodeAdapter()){
        nodeAdapter = (NodeAdapter*) itemAdapter;
    }

    bool updateDeploymentSelection = false;

    if(setSelected){
        if(!selectedIDs.contains(ID)){
            uint itemSelectionID = 0;

            //Get the bitwise shifted Depth + sort order index.
            if(nodeAdapter){
                itemSelectionID = nodeAdapter->getSelectionID();
            }

            //Find spot for this item in the sorted list
            int position = selectedIDs.count();
            for(int i = 0; i < selectedIDs.count(); i++){
                NodeAdapter* nA = getNodeAdapterFromID(selectedIDs[i]);
                if(nA && (itemSelectionID < nA->getSelectionID())){
                    position = i;
                    break;
                }
            }
            selectedIDs.insert(position, ID);

            //Set the Item as Selected
            item->setSelected(true);

            //NodeView
            NodeItemNew * nvi = newNodeItems[ID];
            if(nvi){
                nvi->setSelected(true);
            }



            setActiveSelectionItem(ID);

            //Update the Deployment Selection for any node selected.
            if(nodeAdapter){
                updateDeploymentSelection = true;
            }
        }
    }else{
        //Remove all Items of type;
        selectedIDs.removeAll(ID);

        //Move the selection along before we remove
        if(currentActiveSelectedID == ID){
            setNextActiveSelectionItem();
        }

        //Set the Item as Unselected
        item->setSelected(false);

        NodeItemNew * nvi = newNodeItems[ID];
        if(nvi){
            nvi->setSelected(false);
        }

        //Update the Deployment Selection for any node selected.
        if(nodeAdapter){
            updateDeploymentSelection = true;
        }
    }

    //Update the warnings.
    if(updateDeploymentSelection){
        if(item && isNodeKindDeployable(item->getNodeKind())){
            updateDeploymentWarnings(ID);
        }
    }

    if(selectedIDs.isEmpty()){
        setState(VS_NONE);
    }
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
    //emit view_toggleAspect(VA_ASSEMBLIES, true);
    emit view_toggleAspect(VA_HARDWARE, true);
    // only show a notification if there has been a change in view aspects
    //emit view_DisplayNotification("Turned on deployment view aspects.");
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

    NodeItem* nodeItem = (NodeItem*)item;
    EdgeItem* edgeItem = (EdgeItem*)item;
    EntityItem* entityItem = (EntityItem*)item;

    //All Cases.
    connect(item, SIGNAL(GraphMLItem_ClearSelection()), this, SLOT(clearSelection()));
    connect(item, SIGNAL(GraphMLItem_AppendSelected(GraphMLItem*)), this, SLOT(appendToSelection(GraphMLItem*)));
    connect(item, SIGNAL(GraphMLItem_RemoveSelected(GraphMLItem*)), this, SLOT(removeFromSelection(GraphMLItem*)));
    connect(item, SIGNAL(GraphMLItem_SetActiveSelected(GraphMLItem*)), this, SLOT(setActiveSelectionItem(GraphMLItem*)));
    connect(item, SIGNAL(GraphMLItem_SelectionChanged()), this, SLOT(selectionChanged()));

    connect(item, SIGNAL(GraphMLItem_Hovered(int,bool)), this, SLOT(itemEntered(int,bool)));

    connect(this, SIGNAL(view_ZoomChanged(qreal)), item, SLOT(zoomChanged(qreal)));

    if(item->isEdgeItem()){
        connect(edgeItem, SIGNAL(edgeItem_eventFromItem()), this, SLOT(setEventFromEdgeItem()));
    }

    if(item->isAspectItem()){
        connect(item, SIGNAL(GraphMLItem_SizeChanged()), this, SIGNAL(view_ModelSizeChanged()));
    }

    if(item->isNodeItem()){
        connect(nodeItem, SIGNAL(NodeItem_ResizeSelection(int, QSizeF)), this, SLOT(resizeSelection(int, QSizeF)));
        connect(nodeItem, SIGNAL(NodeItem_ResizeFinished(int)), this, SLOT(resizeFinished(int)));

        connect(this, SIGNAL(view_toggleGridLines(bool)), nodeItem, SLOT(toggleGridMode(bool)));
    }

    if (item->isEntityItem() && entityItem->isHardwareCluster()) {
        connect(Theme::theme(), SIGNAL(theme_Changed()), entityItem, SLOT(themeChanged()));
    }

    if(isMainView()){
        connect(item->getEntityAdapter(), SIGNAL(readOnlySet(int,bool)), this, SLOT(entitySetReadOnly(int,bool)));

        connect(item, SIGNAL(GraphMLItem_SetCentered(GraphMLItem*)), this, SLOT(centerItem(GraphMLItem*)));
        connect(item, SIGNAL(GraphMLItem_TriggerAction(QString)),  this, SLOT(triggerAction(QString)));

        connect(item, SIGNAL(GraphMLItem_SetData(int,QString,QVariant)), this, SIGNAL(view_SetData(int,QString,QVariant)));
        connect(item, SIGNAL(GraphMLItem_DestructData(GraphML*,QString)), this, SIGNAL(view_DestructData(GraphML*,QString)));
        connect(item, SIGNAL(GraphMLItem_ConstructData(GraphML*,QString)), this, SIGNAL(view_ConstructData(GraphML*,QString)));


        if(item->isEntityItem()){
            connect(entityItem, SIGNAL(EntityItem_MoveSelection(QPointF)), this, SLOT(moveSelection(QPointF)));
            connect(entityItem, SIGNAL(EntityItem_MoveFinished()), this, SLOT(moveFinished()));
            connect(entityItem, SIGNAL(EntityItem_HardwareMenuClicked(int)), this, SLOT(hardwareClusterMenuClicked(int)));
            connect(entityItem, SIGNAL(EntityItem_HasFocus(bool)), this, SLOT(editableItemHasFocus(bool)));
            connect(entityItem, SIGNAL(EntityItem_ShowHardwareMenu(EntityItem*)), this, SLOT(showHardwareClusterChildrenViewMenu(EntityItem*)));
            connect(entityItem, SIGNAL(EntityItem_lockMenuClosed(EntityItem*)), this, SLOT(hardwareClusterChildrenViewMenuClosed(EntityItem*)));

            if (entityItem->isHardwareCluster()) {
                connect(this, SIGNAL(view_edgeConstructed()), entityItem, SLOT(updateChildrenViewMode()));
            }
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
    EntityAdapter* graphML = item->getEntityAdapter();
    if(graphML){
        int ID = graphML->getID();
        if(!guiItems.contains(ID)){
            guiItems[ID] = item;
        }
    }
}

void NodeView::removeGraphMLItemFromHash(int ID)
{
    if (guiItems.contains(ID)) {
        GraphMLItem* item = guiItems[ID];
        guiItems.remove(ID);


        if(selectedIDs.contains(ID)){
            selectedIDs.removeAll(ID);
        }
        if(ID == currentActiveSelectedID){
            setNextActiveSelectionItem();
        }

        if(aspectIDs.contains(ID)){
            aspectIDs.removeAll(ID);
        }



        if (item) {
            if (item->isEdgeItem()) {
                EdgeItem* edgeItem = (EdgeItem*)item;
                updateDeployment = true;
                if (edgeItem->getSource() && edgeItem->getDestination()) {
                    emit view_edgeDeleted(edgeItem->getSource()->getID(), edgeItem->getDestination()->getID());
                }
            } else {

                GraphMLItem* parentGraphMLItem = item->getParent();
                int parentID = -1;

                // remove child from parent item
                if (parentGraphMLItem) {
                    parentGraphMLItem->removeChild(ID);
                    parentID = parentGraphMLItem->getID();
                }

                emit view_nodeDeleted(item->getID(), parentID);
            }

            delete item;
        }

        if(IS_SUB_VIEW){
            if(CENTRALIZED_ON_ITEM && centralizedItemID == ID){
                //Delete the nodeView
                deleteLater();
            }
        }


    } else if(noGUIItems.contains(ID)){
        NoGUIItem* item = noGUIItems[ID];
        if(item && item->getEntityAdapter()->isEdgeAdapter()){
            emit view_edgeDeleted();
        }
        noGUIItems.remove(ID);
        delete item;
    }

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

    // send specific current view states to the newly constaructed node item
    entityItem->toggleGridMode(GRID_LINES_ON);

    if (entityItem->getNodeKind().startsWith("Hardware")){
        // this will update the HardwareClusters' children view mode
        // and the HardwareNodes' initial visibility
        updateDeployment = true;

    }

    // if currently pasting, select pasted item
    if (pasting || importFromJenkins) {
        appendToSelection(entityItem);
    }
}






/**
 * @brief NodeView::edgeConstructed_signalUpdates
 * This gets called whenever an edge is constructed.
 * It sends signals to update whatever needs updating.
 * @param edge
 */
void NodeView::edgeConstructed_signalUpdates()
{
    // update the docks
    emit view_edgeConstructed();
}


bool NodeView::isItemsAncestorSelected(GraphMLItem *selectedItem)
{
    EntityAdapter* selectedModelItem = selectedItem->getEntityAdapter();
    if(selectedModelItem->isEdgeAdapter()){
        return false;
    }

    foreach(int ID, selectedIDs){
        GraphMLItem* item = getGraphMLItemFromID(ID);
        EntityAdapter* modelItem = item->getEntityAdapter();

        if(modelItem && modelItem->isNodeAdapter()){
            if(controller){
                if(controller->isNodeAncestor(modelItem->getID(), selectedItem->getID())){
                    return true;
                }
            }
        }
    }
    return false;
}

void NodeView::unsetItemsDescendants(GraphMLItem *selectedItem)
{
    EntityAdapter* selectedModelItem = selectedItem->getEntityAdapter();
    if(selectedModelItem->isEdgeAdapter()){
        return;
    }


    QList<int> currentlySelectedIDs;
    currentlySelectedIDs.append(selectedIDs);

    while(!currentlySelectedIDs.isEmpty()){
        int ID = currentlySelectedIDs.takeFirst();
        GraphMLItem* item = getGraphMLItemFromID(ID);
        EntityAdapter* modelItem  = item->getEntityAdapter();

        bool remove = false;
        if(modelItem && modelItem->isNodeAdapter()){
            remove = controller->isNodeAncestor(selectedModelItem->getID(), modelItem->getID());
        }else if(modelItem && modelItem->isEdgeAdapter()){
            EdgeAdapter* modelEdge = (EdgeAdapter*)modelItem;
            remove = selectedModelItem->getID() == modelEdge->getSourceID() || selectedModelItem->getID() == modelEdge->getDestinationID();
        }
        if(remove){
            setGraphMLItemSelected(item, false);
        }
    }
}

GraphMLItem *NodeView::getSharedEntityItemParent(EntityItem *src, EntityItem *dst)
{
    if(controller){
        int ID = controller->getSharedParent(src->getID(), dst->getID());


        GraphMLItem* node = getGraphMLItemFromID(ID);
        if(node){
            return node;
        }
    }

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

EntityAdapter *NodeView::getEntityAdapterFromID(int ID)
{
    GraphMLItem* gml = getGraphMLItemFromID(ID);
    if(gml){
        return gml->getEntityAdapter();
    }
    return 0;
}

NodeAdapter *NodeView::getNodeAdapterFromID(int ID)
{
    EntityAdapter* eA = getEntityAdapterFromID(ID);
    if(eA && eA->isNodeAdapter()){
        return (NodeAdapter*) eA;
    }
    return 0;
}

NodeItem *NodeView::getNodeItemFromID(int ID)
{
    GraphMLItem* item = getGraphMLItemFromID(ID);
    if(item && item->isNodeItem()){
        return (NodeItem*)item;
    }
    return 0;
}

QString NodeView::getData(int ID, QString key)
{
    QString value;
    if(controller){
        value = controller->getData(ID, key);
    }
    return value;
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

EntityItemNew *NodeView::getEntityItemFromScreenPos(QPoint pos)
{
    QPointF scenePos = mapToScene(pos);

    foreach(QGraphicsItem* item, scene()->items(scenePos)){
        EntityItemNew* eIN =  dynamic_cast<EntityItemNew*>(item);
        if(eIN){
            return eIN;
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
                selectionChanged();
            }
        }
        if(selectedIDs.length() > 0){
            setState(VS_SELECTED);
        }else{
            setState(VS_NONE);
        }

        //Check panning state.
        QLineF distance(panOrigin, event->pos());

        if((distance.length() * getCurrentZoom()) < 5){
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
            lineStart += item->getConnectLineCenter();

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
        QPointF currentPos = mapToScene(event->pos());
        adjustModelPosition(currentPos - panPrevPos);
        //The new previous point needs to be recalculated.
        panPrevPos = mapToScene(event->pos());
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
        //QPointF scenePos = mapToScene(event->pos());
        GraphMLItem* itemUnderMouse = getGraphMLItemFromScreenPos(event->pos());
        EntityItemNew* eIN = getEntityItemFromScreenPos(event->pos());

        switch(viewState){
        case VS_NONE:
            //Handle as Selected.
        case VS_SELECTED:
            //If we are currently in the None/Selected states and there is no item under the mouse/.
            if(!itemUnderMouse && !eIN){
                //If Left Click on no item, clear selection.
                clearSelection();
                selectionChanged();
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
        default:
            break;
        }

        break;
    }
    case Qt::RightButton:{
        switch(viewState){
        case VS_NONE:
            //Handle as Selected.
        case VS_SELECTED:
            setState(VS_PAN);
            panOrigin = event->pos();
            panPrevPos = mapToScene(panOrigin);
            return;
        default:
            break;
        }
        break;
    }
    default:
        break;
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
    QPoint point;

    if(ZOOM_UNDER_MOUSE){
        point = event->pos();
    }

    scrollEvent(event->delta(), point);
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
    EntityItemNew* eIN = getEntityItemFromScreenPos(event->pos());

    if(!item && !eIN){
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
    bool allowedFocusWidget = allowedFocus(focusWidget());

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

        //Handle Tabbage
        if (event->key() == Qt::Key_Tab){
            setNextActiveSelectionItem();
            return;
        }else if(event->key() == Qt::Key_Backtab){
            setNextActiveSelectionItem(true);
            return;
        }

        if (event->key() == Qt::Key_Escape){
            setState(VS_NONE);
            clearSelection(true);
        }
        if(event->key() == Qt::Key_F2){
            if(viewState == VS_NONE || viewState == VS_SELECTED){
                if(currentActiveSelectedID != 0){
                    EntityItem* entityItem = getEntityItemFromID(currentActiveSelectedID);
                    if(entityItem){
                        entityItem->setNewLabel();
                    }
                }
            }
        }
        QPoint arrowAdjust;
        if(event->key() == Qt::Key_Up){
            arrowAdjust.setY(getArrowKeyDelta(SHIFT));
        }

        if(event->key() == Qt::Key_Down){
            arrowAdjust.setY(getArrowKeyDelta(SHIFT, true));
        }

        if(event->key() == Qt::Key_Right){
            arrowAdjust.setX(getArrowKeyDelta(SHIFT, true));

        }

        if(event->key() == Qt::Key_Left){
            arrowAdjust.setX(getArrowKeyDelta(SHIFT));
        }
        if(allowedFocusWidget && !arrowAdjust.isNull()){

            translate(arrowAdjust.x(), arrowAdjust.y());
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
            bool deletePressed = event->key() == Qt::Key_Delete;

            #ifdef Q_OS_MAC
            //Patch for EZ delete on OSX
            deletePressed = event->key() == Qt::Key_Backspace;
            #endif

            if(deletePressed){
                deleteSelection();
            }
        }
        QGraphicsView::keyReleaseEvent(event);

    }

    void NodeView::setReadOnlyMode(bool readOnly)
    {
        if (selectedIDs.count() > 0) {
            if(viewMutex.tryLock()){
                triggerAction("Setting Read Only.");
                emit view_SetReadOnly(selectedIDs, readOnly);
            }
        } else {
            view_DisplayNotification("Select entity(s) to copy.");
        }
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
            view_DisplayNotification("No selected entities to align.");
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
            view_DisplayNotification("No selected entities to align.");
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
                    view_DisplayNotification("Cannot align entities which aren't contained by the same parent.");
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
        QGraphicsView::setEnabled(enabled);
    }


    void NodeView::showMessage(MESSAGE_TYPE type, QString messageString, QString messageTitle, QString messageIcon, int centerID)
    {
        GraphMLItem* item = getGraphMLItemFromID(centerID);
        if(item){
            centerItem(item);
        }
        if(messageString != ""){
            if(type == CRITICAL){
                QMessageBox msgBox(QMessageBox::Critical, messageTitle, messageString);
                msgBox.setWindowIcon(Theme::theme()->getImage("Actions", "Critical", QSize(), Qt::black));
                if(messageIcon != ""){
                    msgBox.setIconPixmap(Theme::theme()->getImage("Actions", messageIcon, QSize(64,64), Qt::black));
                }
                msgBox.exec();
            }else{
                emit view_DisplayNotification(messageString, messageIcon);
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
            view_DisplayNotification("Select entity(s) to replicate.");
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
            view_DisplayNotification("Select entity(s) to copy.");
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
                //setAttributeModel(0, true);
                emit view_Cut(selectedIDs);
            }
        } else {
            view_DisplayNotification("Select entity(s) to cut.");
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
            //view_DisplayNotification("Select entity to paste into.");
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
                    appendToSelection(child);
                }
            }
            selectionChanged();
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
            emit view_Undo();
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
        if (!item) {
            return;
        }

        if (isItemsAncestorSelected(item)) {
            return;
        }

        //Unset Items Descendant Items.
        unsetItemsDescendants(item);

        //Set this item as Selected.
        setGraphMLItemSelected(item, true);

        // update enabled states of menu and toolbar actions
        if (updateActions) {
            selectionChanged();
        }
    }

    void NodeView::setActiveSelectionItem(GraphMLItem *item)
    {
        if(!item){
            return;
        }
        setActiveSelectionItem(item->getID());
    }

    /**
     * @brief NodeView::removeFromSelection
     * @param item
     */
    void NodeView::removeFromSelection(GraphMLItem *item, bool updateActions)
    {
        //Set this item as Selected.
        setGraphMLItemSelected(item, false);

        if(updateActions){
            selectionChanged();
        }
    }

    void NodeView::moveSelection(QPointF delta)
    {
        if(viewState == VS_SELECTED){
            setState(VS_MOVING);
        }
        if(viewState == VS_MOVING){

            bool canReduceX = true;
            bool canReduceY = true;

            foreach(int ID, selectedIDs){
                GraphMLItem* graphMLItem = getGraphMLItemFromID(ID);
                EntityAdapter* graphml = graphMLItem->getEntityAdapter();

                if(graphml && graphml->isNodeAdapter()){
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

                        NodeItemNew*  nvi = newNodeItems[ID];
                        if(nvi){
                            nvi->getParentNodeItem()->setGridVisible(true);
                        }

                    }

                    nodeItem->adjustPos(delta);

                    NodeItemNew*  nvi = newNodeItems[ID];
                    if(nvi){
                        nvi->adjustPos(delta);
                    }
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
        //triggerAction("RESIZE YO");
        foreach(int ID, selectedIDs){
            GraphMLItem* graphMLItem = getGraphMLItemFromID(ID);
            if(graphMLItem && graphMLItem->isNodeItem()){
                NodeItem* nodeItem = (NodeItem*) graphMLItem;

                if(nodeItem->getParentNodeItem()){
                    nodeItem->getParentNodeItem()->setDrawGrid(false);

                    NodeItemNew*  nvi = newNodeItems[ID];
                    if(nvi){
                         nvi->getParentNodeItem()->setGridVisible(false);
                    }

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
            //triggerAction("RESIZE YO");
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
    void NodeView::clearSelection(bool updateActions)
    {
        while (!selectedIDs.isEmpty()){
            int currentID = selectedIDs.takeFirst();
            GraphMLItem* currentItem = getGraphMLItemFromID(currentID);
            if (currentItem) {
                setGraphMLItemSelected(currentItem, false);
            }
        }



        ModelItem* modelItem = getModelItem();
        if(modelItem){
            //Allow defocusing of other things
            modelItem->setFocus();
        }


        if(updateActions){
            selectionChanged();
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
        SELECT_AFTER_CONSTRUCTION = select;
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

        selectionChanged();
    }

    void NodeView::constructEntityItem(EntityAdapter *item)
    {

        if(isSubView() && controller){
            int ID = item->getID();
            if(!controller->areIDsInSameBranch(centralizedItemID, ID)){
                return;
            }
        }

        bool constructed = false;
        if(item && item->isInModel()){
            if(getGraphMLItemFromID(item->getID())){
                //Only construct each item once.
                return;
            }

            if(item->isNodeAdapter()){
                constructed = constructNodeItem((NodeAdapter*) item);
            }else if(item->isEdgeAdapter()){
                constructed = constructEdgeItem((EdgeAdapter*) item);
            }
        }
        if(!constructed){
            constructNoGUIItem(item);
        }
    }

    void NodeView::destructEntityItem(EntityAdapter *item)
    {
        int deployedID = -1;
        bool _updateDeploymentWarnings = false;

        if(item && item->isEdgeAdapter()){
            EdgeAdapter* eA = (EdgeAdapter*)item;
            Edge::EDGE_CLASS edgeClass = eA->getEdgeClass();

            if(edgeClass == Edge::EC_DEPLOYMENT){
                _updateDeploymentWarnings = true;
                deployedID = eA->getSourceID();
            }
        }

        destructGUIItem(item->getID());

        if(_updateDeploymentWarnings){
            updateDeploymentWarnings(deployedID);
        }
    }

    bool NodeView::constructNodeItem(NodeAdapter *node)
    {
        if(!node){
            return false;
        }

        NODE_CLASS nodeClass = node->getNodeClass();

        //Check if we should construct this Node.
        if(nonDrawnNodeClasses.contains(nodeClass)){
            return false;
        }

        GraphMLItem* visibleParentItem = 0;

        int depth = 1;
        while(true){
            int parentID = node->getParentNodeID(depth);
            if(parentID == -1){
                break;
            }
            GraphMLItem* parentItem = getGraphMLItemFromID(parentID);
            if(parentItem){
                visibleParentItem = parentItem;
                break;
            }
            depth ++;
        }


        NodeItem* parentNodeItem = 0;
        EntityItem* parentEntityItem = 0;



        if(visibleParentItem){
            if(visibleParentItem->isEntityItem()){
                parentEntityItem = (EntityItem*) visibleParentItem;
            }
            parentNodeItem = dynamic_cast<NodeItem*>(visibleParentItem);
        }

        QVariant xVal = node->getDataValue("x");
        QVariant yVal = node->getDataValue("y");
        QVariant wVal = node->getDataValue("width");
        QVariant hVal = node->getDataValue("height");

        QString nodeKind = node->getDataValue("kind").toString();


        QRectF itemRect;
        QPointF centerPoint;
        QSizeF size;

        if(parentNodeItem){
            itemRect = parentNodeItem->getChildBoundingRect();
        }

        if(wVal.isValid() && hVal.isValid()){
            qreal w = wVal.toDouble();
            qreal h = hVal.toDouble();

            if(w > itemRect.width()){
                itemRect.setWidth(w);
            }
            if(h > itemRect.height()){
                itemRect.setHeight(h);
            }
        }

        size = itemRect.size();

        if(xVal.isValid() && yVal.isValid()){
            qreal x = xVal.toDouble();
            qreal y = yVal.toDouble();

            if(x == -1 && y == -1){
                if(parentNodeItem){
                    centerPoint = parentNodeItem->getNextChildPos(itemRect);
                }
            }else{
                centerPoint = QPointF(x,y);
            }
        }


        bool expandItem = toolbarDockConstruction || importFromJenkins;

        //Expand Parent
        if(expandItem && parentEntityItem){
            emit view_SetData(parentEntityItem->getID(), "isExpanded", true);
        }



        GraphMLItem* item = 0;

        if(nodeKind == "Model"){
            item = new ModelItem(node, this);
        }else if(node->isAspect()){
            VIEW_ASPECT aspect = GET_ASPECT_FROM_KIND(nodeKind);
            if(aspect != VA_NONE){
                item = new AspectItem(node, visibleParentItem, aspect);
                aspectIDs << node->getID();
            }else{
                qCritical() << "Unknown Aspect!";
                return false;
            }
        }else{
            item =  new EntityItem(node, parentNodeItem);
        }

        //Do Generic connect stuffs.
        if(item){
            storeGraphMLItemInHash(item);

            if(!scene()->items().contains(item)){
                scene()->addItem(item);
            }

            connectGraphMLItemToController(item);


            EntityItem* entityItem = (EntityItem*) item;
            NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);

            if(nodeItem && !size.isNull()){
                nodeItem->setWidth(size.width());
                nodeItem->setHeight(size.height());
            }
            if(item->isEntityItem()){
                entityItem->setCenterPos(centerPoint);
                QPair<QString, bool> editField = getEditableDataKeyName(entityItem);
                QPair<QString, bool> statusField = getStatusDataKeyName(entityItem);

                entityItem->setNodeConnectable(isNodeVisuallyConnectable(node));
                entityItem->setEditableField(editField.first, editField.second);
                entityItem->setStatusField(statusField.first, statusField.second);
            }

            if(parentNodeItem){
                parentNodeItem->updateSizeInModel();
            }
            if(nodeItem){
                //Update the model if we need to.
                nodeItem->updateSizeInModel();
                nodeItem->updatePositionInModel();
                // send/do necessary signals/updates when a node has been constructed
                nodeConstructed_signalUpdates(nodeItem);
            }




            // if SELECT_ON_CONSTRUCTION, select node after construction and center on it
            // the node's label is automatically selected and editable
            if (toolbarDockConstruction && SELECT_AFTER_CONSTRUCTION) {
                clearSelection();
                // why not update menu/toolbar actions here?
                appendToSelection(item, true);

                if(item->isEntityItem()){
                    //Set new Label for EntityItem.
                    entityItem->setNewLabel();
                }

                centerOnItem();
                toolbarDockConstruction = false;
            }

            if(isSubView() && item->getID() == centralizedItemID){
                appendToSelection(item);
            }
        }else{
            return false;
        }
        return true;
    }



    void NodeView::destructGUIItem(int ID)
    {
        removeGraphMLItemFromHash(ID);
    }


    /**
     * @brief NodeView::showManagementComponents
     * @param show
     */
    void NodeView::showManagementComponents(bool show)
    {
        if(!controller){
            return;
        }

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
            //viewDeploymentAspect();
            notificationNumber++;
        }  else {
            numberOfNotifications = 1;
        }

        /*if (show) {
            view_DisplayNotification("Displayed Management Components.", notificationNumber, numberOfNotifications);
        } else {
            view_DisplayNotification("Hidden Management Components.", notificationNumber, numberOfNotifications);
        }*/

        // this goes through all the ManagementComponents and shows/hides them
        foreach (Node* node, assemblyDefinition->getChildren(0)){
            EntityItem* entityItem = getEntityItemFromNode(node);
            if(entityItem && entityItem->getNodeKind() == "ManagementComponent"){
                entityItem->setHidden(!show);
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
        AspectItem* aspect =  getAspectItem(VA_HARDWARE);
        if(aspect){
            EntityItem* localHostNode = 0;
            foreach(GraphMLItem* child, aspect->getChildren()){
                if(child->isEntityItem() && child->getNodeKind() == "HardwareNode"){
                    localHostNode = (EntityItem*) child;
                    break;
                }
            }
            if(localHostNode){
                localHostNode->setHidden(!show);
                aspect->sortChildren();
            }
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
        ZOOM_UNDER_MOUSE = underMouse;
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
            if (modelItem->getVisibleAspects().isEmpty()) {
                itemsToCenter.append(getModelItem());
            } else {
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


    void NodeView::constructWorkerProcessNode(QString workerName, QString operationName, int sender)
    {
        //Do stuff!
        if(viewMutex.tryLock()){
            NodeItem* item = getSelectedNodeItem();
            if (item) {
                toolbarDockConstruction = true;

                QPointF position = item->getNextChildPos();

                if(sender == 1){
                    // if from toolbar, place at closest grid point to the toolbar's position
                    position = item->mapFromScene(toolbarPosition);
                    position = item->getClosestGridPoint(position);
                }
                emit view_ConstructWorkerProcessNode(item->getID(), workerName, operationName, position);
            }
        }
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
        foreach (EntityItem* entityItem, getEntityItemsList()) {
            if (entityItem) {
                entityItem->resetSize();
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

    bool NodeView::constructEdgeItem(EdgeAdapter *edge)
    {
        int srcID = edge->getSourceID();
        int dstID = edge->getDestinationID();

        EntityItem* srcGUI = getEntityItemFromGraphMLItem(getGraphMLItemFromID(srcID));
        EntityItem* dstGUI = getEntityItemFromGraphMLItem(getGraphMLItemFromID(dstID));

        bool updateDeploymentWarning = false;
        Edge::EDGE_CLASS edgeClass = edge->getEdgeClass();
        if(edgeClass == Edge::EC_DEPLOYMENT){
            updateDeployment = true;
            updateDeploymentWarning = true;
        }

        // need to put this back here
        edgeConstructed_signalUpdates();

        bool constructEdge = srcGUI != 0 && dstGUI != 0;

        if(constructEdge){
            //We have valid GUI elements for both ends of this edge.

            switch(edgeClass){
            case Edge::EC_AGGREGATE:
                constructEdge = false;
                break;
            case Edge::EC_DEFINITION:
                constructEdge = false;
                break;
            case Edge::EC_DEPLOYMENT:
                constructEdge = false;
            default:
                break;
            }

            GraphMLItem* parent = getSharedEntityItemParent(srcGUI, dstGUI);

            if(!parent || !parent->isNodeItem()){
                // added this here otherwise constructed edge with no gui is not stored
                constructEdge = false;
            }

            if(constructEdge){
                NodeItem* parentNodeItem = (NodeItem*) parent;

                //Construct a new GUI Element for this edge
                EdgeItem* nodeEdge = new EdgeItem(edge, parentNodeItem, srcGUI, dstGUI);

                //Add it to the list of EdgeItems in the Model.
                storeGraphMLItemInHash(nodeEdge);
                connectGraphMLItemToController(nodeEdge);

                if(!scene()->items().contains(nodeEdge)){
                    //Add to model.
                    scene()->addItem(nodeEdge);
                }
            }
        }

        //Update deployment Warnings
        if(!constructEdge && updateDeploymentWarning){
            updateDeploymentWarnings(srcID);
        }

        return constructEdge;
    }

    void NodeView::constructNoGUIItem(EntityAdapter *entity)
    {
        NoGUIItem* item = new NoGUIItem(entity);
        int ID = item->getID();
        noGUIItems[ID] = item;
    }


