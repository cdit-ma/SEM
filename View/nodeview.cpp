#include "nodeview.h"
#include "../Controller/controller.h"
#include "Toolbar/toolbarwidget.h"
#include "Dock/docktogglebutton.h"
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

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR
#define MIN_ZOOM 0.01
#define MAX_ZOOM 1.5

#define MAX_ZOOM_RATIO 50
#define MIN_ZOOM_RATIO 2

#define VIEW_PADDING 1.1


/**
 * @brief NodeView::NodeView
 * @param subView
 * @param parent
 */
NodeView::NodeView(bool subView, QWidget *parent):QGraphicsView(parent)
{
    constructedFromImport = true;
    toolbarJustClosed = false;
    editingNodeItemLabel = false;
    managementComponentVisible = false;
    IS_SUB_VIEW = subView;
    controller = 0;
    parentNodeView = 0;
    rubberBand = 0;

    prevLockMenuOpened = 0;

    CENTRALIZED_ON_ITEM = false;
    CONTROL_DOWN = false;
    SHIFT_DOWN = false;
    IS_RESIZING = false;
    IS_MOVING = false;

    setScene(new QGraphicsScene(this));

    //Set QT Options for this QGraphicsView
    setDragMode(ScrollHandDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //Set GraphicsView background-color
    setStyleSheet("QGraphicsView{ background-color: rgba(175,175,175,255); border: 0px;}");

    /*
    scene()->setBackgroundBrush(QBrush(Qt::white));
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("QGraphicsView{ background-color: rgba(255,255,255,200); border: 0px;}");
    */

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

    // initialise the view's center point
    centerPoint = getVisibleRect().center();

    currentMapKey = -1;
    initialRect = 0;
    viewMovedBackForward = false;
}


void NodeView::setController(NewController *c)
{
    controller = c;
}

void NodeView::disconnectController()
{
    controller = 0;
}

bool NodeView::isSubView()
{
    return IS_SUB_VIEW;
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
    //Clear the current Selected Attribute Model so that the GUI doesn't crash.
    view_SetAttributeModel(0);

    if(this->parentNodeView){
        parentNodeView->removeSubView(this);
        centralizedItemID = "";
        CENTRALIZED_ON_ITEM = false;
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
    if (getModelItem()) {
        getModelItem()->adjustPos(deltaPos);
        updateViewCenterPoint();
    }
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
    if (getModelItem()) {
        getModelItem()->adjustPos(deltaPos);
        updateViewCenterPoint();
    }

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
    NodeItem* modelItem = getNodeItemFromNode(controller->getModel());
    return modelItem;
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
 * @brief NodeView::getSelectedNode
 * This returns the currently selected node.
 * If there are multiple nodes selected, it returns null.
 * @return
 */
Node* NodeView::getSelectedNode()
{
    if (selectedIDs.size() == 1) {
        QString ID = selectedIDs[0];
        GraphMLItem* item = getGraphMLItemFromHash(ID);
        if (item->isNodeItem()) {
            return (Node*)item->getGraphML();
        }
    }
    return 0;
}


/**
 * @brief NodeView::getSelectedNodeItem
 * This returns the currently selected node item based on getSelectedNode().
 * @return
 */
NodeItem* NodeView::getSelectedNodeItem()
{
    NodeItem* selectedItem = getNodeItemFromNode(getSelectedNode());
    return selectedItem;
}


void NodeView::setParentNodeView(NodeView *n)
{
    this->parentNodeView = n;
}

void NodeView::removeSubView(NodeView *subView)
{
    if(subViews.contains(subView)){
        qCritical() << "REMOVING SUBVIEW";
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


QStringList NodeView::getAdoptableNodeList(Node* node)
{
    return controller->getAdoptableNodeKinds(node);
}


QList<Node*> NodeView::getConnectableNodes(Node* node)
{
    return controller->getConnectableNodes(node);
}


void NodeView::constructNewView(Node* centeredOn)
{
    if(IS_SUB_VIEW){
        //Cannot make subviews of subviews.
        return;
    }

    if(!centeredOn){
        //return;

        // added this here and added a default for centeredOn
        // so that MEDEA's toolbar can use this slot
        centeredOn = getSelectedNode();
    }

    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;

    QDialog* newViewWindow = new QDialog(this, flags);
    newViewWindow->setWindowModality(Qt::NonModal);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    Node* firstNode = centeredOn;

    NodeView* newView = new NodeView(true, newViewWindow);

    connect(newViewWindow, SIGNAL(rejected()),newView, SLOT(deleteLater()));
    this->subViews.append(newView);
    newView->setParentNodeView(this);

    mainLayout->addWidget(newView);
    newViewWindow->setLayout(mainLayout);
    newViewWindow->show();

    if(this->controller){
        controller->connectView(newView);

        QStringList newAspects = currentAspects;
        if (centeredOn->isDefinition() && !newAspects.contains("Definitions")) {
            newAspects.append("Definitions");
        }
        if (centeredOn->isImpl() && !newAspects.contains("Behaviour")) {
            newAspects.append("Behaviour");
        }
        newView->setAspects(newAspects);


        QList<Node*> toConstruct;
        toConstruct << centeredOn->getChildren();

        while(centeredOn){
            toConstruct.insert(0,centeredOn);
            centeredOn = centeredOn->getParentNode();
        }

        while(toConstruct.size() > 0){
            newView->constructGUIItem(toConstruct.takeFirst());;
        }
        newView->view_LockCenteredGraphML(firstNode);

    }
}


bool NodeView::viewportEvent(QEvent * e)
{
    emit view_ViewportRectChanged(getVisibleRect());
    return QGraphicsView::viewportEvent(e);
}


void NodeView::minimapPan(QPointF delta)
{
    ViewportAnchor currentAnchor = transformationAnchor();
    setTransformationAnchor(NoAnchor);
    translate(delta.x(), delta.y());
    setTransformationAnchor(currentAnchor);
}


void NodeView::setRubberBandMode(bool On)
{

    if(!rubberBand){
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    }
    if(On && !IS_SUB_VIEW){
        RUBBERBAND_MODE = true;
        setDragMode(RubberBandDrag);
    }else{
        RUBBERBAND_MODE = false;
        drawingRubberBand = false;
        rubberBand->setVisible(false);
        setDragMode(ScrollHandDrag);
    }
}


/**
 * @brief NodeView::clearView
 * This is called when the a new project is created. It clears the scene.
 */
void NodeView::clearView()
{
    scene()->clear();
    viewport()->update();
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
void NodeView::sortNode(Node *node, Node* topMostNode)
{
    if (!topMostNode) {
        triggerAction("View: Sorting Node");
        topMostNode = node;
    }

    NodeItem* nodeItem = getNodeItemFromNode(node);

    // check if node has children
    if (nodeItem->getChildNodeItems().count() == 0) {
        // if it doesn't, iterate up to the topMost node
        while (nodeItem->getParentNodeItem()) {
            //  if current node == topMost node, return
            if (nodeItem->getNode() == topMostNode) {
                break;
            }
            // otherwise, iterate sorting upto the topMost node
            nodeItem = nodeItem->getParentNodeItem();
            //nodeItem->sort();
            nodeItem->newSort();
        }
    } else {
        // go to the lowest level child and start sorting from there
        foreach (NodeItem* child, nodeItem->getChildNodeItems()) {
            sortNode(child->getNode(), topMostNode);
        }
    }
}


/**
 * @brief NodeView::sortEntireModel
 * This recursively sorts the enitre model.
 */
void NodeView::sortEntireModel()
{
    sortNode(controller->getModel());
}


/**
 * @brief NodeView::centerItem
 * This scales and translates the view to fit and center on the item.
 * @param item - graphics item to center on
 */
void NodeView::centerItem(GraphMLItem *item)
{
    if (item) {
        centerRect(item->sceneBoundingRect());
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
        item = guiItems[ID];
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
    }
}


/**
 * @brief NodeView::showNodeItemLockMenu
 * This slot shows/hides the provided nodeItem's lock menu.
 * @param nodeItem
 */
void NodeView::showNodeItemLockMenu(NodeItem* nodeItem)
{
    QMenu* menu = nodeItem->getLockMenu();
    if (menu) {
        if (menu == prevLockMenuOpened) {
            menu->close();
            prevLockMenuOpened = 0;
        } else {
            QRectF lockRect = nodeItem->getLockIconSceneRect();
            QPoint offset(lockRect.width()/5, -lockRect.width()/15);
            QPointF menuPos = mapFromScene(lockRect.bottomLeft() + offset);
            menuPos = mapToGlobal(menuPos.toPoint());
            menu->popup(menuPos.toPoint());
            prevLockMenuOpened = menu;
        }
    }
}


/**
 * @brief NodeView::nodeItemLockMenuClosed
 * This checks to see if the nodeItem's lock menu was closed by clicking on lock icon.
 * If it wasn't, reset prevLockMenuOpened so that showNodeItemLockMenu works correctly.
 * @param nodeItem
 */
void NodeView::nodeItemLockMenuClosed(NodeItem* nodeItem)
{
    QPointF viewPos = mapFromScene(nodeItem->getLockIconSceneRect().topLeft());
    QPointF globalPos = mapToGlobal(viewPos.toPoint());
    QRectF rect(globalPos, nodeItem->getLockIconSceneRect().size()*transform().m11());

    if (!rect.contains(QCursor::pos())) {
        prevLockMenuOpened = 0;
        //emit view_nodeItemLockMenuClosed(nodeItem);
    }
}


/**
 * @brief NodeView::keepSelectionFullyVisible
 * This is called whenever a node item is moved or resized.
 * If the item is selected, adjust the view rect accordingly to
 * fit the whole item then center on the adjusted view rect.
 * @param item - graphics item that was changed
 */
void NodeView::keepSelectionFullyVisible(GraphMLItem* item)
{
    if (!item || !selectedIDs.contains(item->getID())) {
        return;
    }

    QRectF origViewRect = getVisibleRect();
    QRectF viewRect = origViewRect;
    QRectF itemRect = item->sceneBoundingRect();

    // translate the view rect to fit the selected item(s)
    if (itemRect.top() < viewRect.top()) {
        viewRect.translate(0, itemRect.top() - viewRect.top());
    }
    if (itemRect.bottom() > viewRect.bottom()) {
        viewRect.translate(0, itemRect.bottom() - viewRect.bottom());
    }
    if (itemRect.left() < viewRect.left()) {
        viewRect.translate(itemRect.left() - viewRect.left(), 0);
    }
    if (itemRect.right() > viewRect.right()) {
        viewRect.translate(itemRect.right() - viewRect.right(), 0);
    }

    // grow the view rect to fit the selected item(s)
    /*
    if (itemRect.top() < viewRect.top()) {
        viewRect.setTop(itemRect.top());
    }
    if (itemRect.bottom() > viewRect.bottom()) {
        viewRect.setBottom(itemRect.bottom());
    }
    if (itemRect.left() < viewRect.left()) {
        viewRect.setLeft(itemRect.left());
    }
    if (itemRect.right() > viewRect.right()) {
        viewRect.setRight(itemRect.right());
    }
    */

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
 * @brief NodeView::showToolbar
 * This is called when there is a mouse right-click event.
 * It show/hides the context or window toolbar depending on where the right click happened.
 * @param position
 */
void NodeView::showToolbar(QPoint position)
{
    // use mouse click position when constructing node items from toolbar
    QPoint globalPos = mapToGlobal(position);
    toolbarPosition = mapToScene(position);

    // only show the toolbar if there is at least one node item selected
    if (selectedIDs.count() > 0) {

        // get all the selected node items
        QList<NodeItem*> selectedItems;
        foreach (NodeItem* item, getNodeItemsList()) {
            if (item->isPainted() && selectedIDs.contains(item->getID())) {
                selectedItems.append(item);
            }
        }

        // find out if the user right-clicked on one of the selected items
        bool toolbarPositionContained = false;
        foreach (NodeItem* selectedItem, selectedItems) {
            if (selectedItem->sceneBoundingRect().contains(toolbarPosition)) {
                toolbarPositionContained = true;
                break;
            }
        }

        // only show the toolbar if the right-click happened inside one of the selected items
        if (toolbarPositionContained) {
            toolbar->updateSelectedNodeItem(selectedItems);
            toolbar->move(globalPos);
            toolbar->show();
        }
    }

    // show/hide MEDEA toolbar
    //view_showWindowToolbar();
}


void NodeView::view_ConstructNodeGUI(Node *node)
{
    if(!node){
        qCritical() << "Node is Null.";
    }

    Node* parentNode = node->getParentNode();

    QString nodeKind = node->getDataValue("kind");

    if(nonDrawnItemKinds.contains(nodeKind)){
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

    if(!constructedFromImport){
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
    bool resetSize = xPos == yPos && xPos == "-1";

    //qreal multiplier = 1;
    if(resetSize){
        QPointF newPosition;
        if(parentNodeItem){
            newPosition = parentNodeItem->getNextChildPos();
        }
        node->getData("x")->setValue(QString::number(newPosition.x()));
        node->getData("y")->setValue(QString::number(newPosition.y()));
    }


    NodeItem* nodeItem = new NodeItem(node, parentNodeItem, currentAspects, IS_SUB_VIEW);


    nodeItem->setGraphicsView(this);
    storeGraphMLItemInHash(nodeItem);


    //Connect the Generic Functionality.
    connectGraphMLItemToController(nodeItem, node);



    // send/do necessary signals/updates when a node has been constructed
    nodeConstructed_signalUpdates(nodeItem);

    // if SELECT_ON_CONSTRUCTION, select node after construction and center on it
    // the node's label is automatically selected and editable
    if(!constructedFromImport){
        if(GRID_LINES_ON){
            nodeItem->snapToGrid();
        }
        if(SELECT_ON_CONSTRUCTION){
            clearSelection(true, false);
            appendToSelection(nodeItem);
            nodeItem->setNewLabel();
            centerOnItem();
        }
    }

    // only do the SELECT_ON_CONSTRUCTION stuff if the node was constructed
    // using the dock/toolbar and not from GraphML import - reset check
    if (!constructedFromImport) {
        constructedFromImport = true;
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

    NodeItem* srcGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(src->getID()));
    NodeItem* dstGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(dst->getID()));

    if(srcGUI != 0 && dstGUI != 0){
        //We have valid GUI elements for both ends of this edge.

        //Construct a new GUI Element for this edge.
        EdgeItem* nodeEdge = new EdgeItem(edge, srcGUI, dstGUI);

        //Add it to the list of EdgeItems in the Model.
        storeGraphMLItemInHash(nodeEdge);

        connectGraphMLItemToController(nodeEdge, edge);

        // show hidden hardware node before the new edge is added to scene
        // sort its parent so that the newly visible hardware node can be easily seen
        if (dstGUI->getGraphML()->getDataValue("kind") == "HardwareNode") {
            dstGUI->setHidden(false);
            if (dstGUI->getParentNodeItem()) {
                dstGUI->getParentNodeItem()->newSort();
            }
        }


        if(!scene()->items().contains(nodeEdge)){
            //Add to model.
            scene()->addItem(nodeEdge);
        }

        // send necessary signals when an edge has been constucted
        edgeConstructed_signalUpdates(edge);


    }else{
        if(!IS_SUB_VIEW){
            qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
        }
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

void NodeView::view_LockCenteredGraphML(GraphML *graphML)
{
    if(!graphML){
        return;
    }

    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(guiItem);
    if(nodeItem){
        centralizedItemID = graphML->getID();

        centerItem(guiItem);
        CENTRALIZED_ON_ITEM = true;
        nodeItem->setPermanentlyCentralized(true);

        connect(nodeItem, SIGNAL(recentralizeAfterChange(GraphML*)), this, SLOT(view_CenterGraphML(GraphML*)));

    }
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
    triggerAction("Dock/Toolbar: Constructing Node");

    NodeItem* selectedItem = getSelectedNodeItem();
    if (selectedItem) {
        constructedFromImport = false;
        if (sender == 0) {
            // if from dock, place at next available position on grid
            view_ConstructNode(selectedItem->getNode(), nodeKind, selectedItem->getNextChildPos());
        } else if (sender == 1) {
            // if from toolbar, place at closest grid point to the toolbar's position
            QPointF position = selectedItem->mapFromScene(toolbarPosition);
            QPointF newPosition = selectedItem->getClosestGridPoint(position);
            view_ConstructNode(selectedItem->getNode(), nodeKind, newPosition);
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
void NodeView::constructEdge(Node* src, Node* dst, bool trigger)
{
    emit view_displayNotification("Connected " + src->getDataValue("label") +
                                  " to " + dst->getDataValue("label"));
    if (trigger) {
        triggerAction("Dock/Toolbar: Constructing Edge");
    }
    view_ConstructEdge(src, dst);
}


/**
 * @brief NodeView::constructConnectedNode
 * @param parentNode - selected node's parent node
 * @param node - selected node
 * @param kind - node kind to construct
 * @param sender - 0 = DockScrollArea, 1 = ToolbarWidget
 */
void NodeView::constructConnectedNode(Node* parentNode, Node* node, QString kind, int sender)
{
    triggerAction("Dock/Toolbar: Constructing Connected Node");

    if (parentNode && node) {
        NodeItem *nodeItem = getNodeItemFromNode(parentNode);
        constructedFromImport = false;
        if (sender == 0) {
            view_ConstructConnectedComponents(parentNode, node, kind, nodeItem->getNextChildPos());
        } else if (sender == 1) {
            view_ConstructConnectedComponents(parentNode, node, kind, nodeItem->mapFromScene(toolbarPosition));
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
        foreach (Edge* edge, node->getEdges()) {
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


/**
 * @brief NodeView::componentInstanceConstructed
 * This is called when a ComponentInstance is created with a definition.
 * It makes sure that ComponentInstance is selected if the SELECT_ON_CONSTRUCTION is turned on.
 * @param node
 */
void NodeView::componentInstanceConstructed(Node* node)
{
    if (SELECT_ON_CONSTRUCTION) {
        NodeItem* nodeItem = getNodeItemFromNode(node);
        if (nodeItem) {
            clearSelection(true, false);
            appendToSelection(nodeItem);
            nodeItem->setNewLabel();
            centerOnItem();
        }
    }
}


/**
 * @brief NodeView::destructEdge
 * This is called when an edge is destroyed.
 * @param edge
 */
void NodeView::destructEdge(Edge* edge)
{
    emit view_displayNotification("Disconnected " + edge->getSource()->getDataValue("label") +
                                  " from " + edge->getDestination()->getDataValue("label"));
    view_Delete(QStringList() << edge->getID());
}


void NodeView::setGraphMLItemSelected(GraphMLItem *item, bool setSelected)
{
    QString itemID = item->getGraphML()->getID();

    if(setSelected){
        if(!selectedIDs.contains(itemID)){
            selectedIDs.append(itemID);
            item->setSelected(true);
            if(item->isNodeItem()){
                NodeItem* nodeItem = (NodeItem*) item;
                nodeSelected_signalUpdates(nodeItem->getNode());
            }
            emit view_SetAttributeModel(item->getAttributeTable());
        }
    }else{
        if(selectedIDs.contains(itemID)){
            emit view_SetAttributeModel(0);
            selectedIDs.removeAll(itemID);


            if(selectedIDs.size() > 0){
                GraphMLItem* item = getGraphMLItemFromHash(selectedIDs.last());
                if(item){
                    view_SetAttributeModel(item->getAttributeTable());
                }
            }
            item->setSelected(false);
        }
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
 * @brief NodeView::getConstructableNodeKinds
 * This method gets the constuctable node kinds list from the controller.
 * @return
 */
QStringList NodeView::getConstructableNodeKinds()
{
    return controller->getConstructableNodeKinds();
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
        emit view_displayNotification("Turned on Deployment view aspects");
    }
    addAspect("Assembly");
    addAspect("Hardware");
}


void NodeView::connectGraphMLItemToController(GraphMLItem *GUIItem, GraphML *graphML)
{
    if(GUIItem){


        NodeItem* nodeItem = 0;
        if(GUIItem->isNodeItem()){
            nodeItem = (NodeItem*) GUIItem;
        }

        if(GUIItem){
            connect(GUIItem, SIGNAL(GraphMLItem_ClearSelection(bool)), this, SLOT(clearSelection(bool)));
            connect(GUIItem, SIGNAL(GraphMLItem_AppendSelected(GraphMLItem*)), this, SLOT(appendToSelection(GraphMLItem*)));
            connect(GUIItem, SIGNAL(GraphMLItem_RemoveSelected(GraphMLItem*)), this, SLOT(removeFromSelection(GraphMLItem*)));
            connect(GUIItem, SIGNAL(GraphMLItem_SetCentered(GraphMLItem*)), this, SLOT(centerItem(GraphMLItem*)));
            connect(GUIItem, SIGNAL(GraphMLItem_PositionSizeChanged(GraphMLItem*)), this, SLOT(keepSelectionFullyVisible(GraphMLItem* )));
            //connect(GUIItem, SIGNAL(GraphMLItem_MovedOutOfScene(GraphMLItem*)), this, SLOT(fitInSceneRect(GraphMLItem*)));
            connect(this, SIGNAL(view_AspectsChanged(QStringList)), GUIItem, SLOT(aspectsChanged(QStringList)));
        }

        if(!IS_SUB_VIEW){
            connect(GUIItem, SIGNAL(GraphMLItem_TriggerAction(QString)),  this, SLOT(triggerAction(QString)));

            connect(GUIItem, SIGNAL(GraphMLItem_SetGraphMLData(GraphML*,QString,QString)), this, SIGNAL(view_SetGraphMLData(GraphML*,QString,QString)));
            connect(GUIItem, SIGNAL(GraphMLItem_ConstructGraphMLData(GraphML*,QString)), this, SIGNAL(view_ConstructGraphMLData(GraphML*,QString)));
            connect(GUIItem, SIGNAL(GraphMLItem_DestructGraphMLData(GraphML*,QString)), this, SIGNAL(view_DestructGraphMLData(GraphML*,QString)));

            connect(GUIItem, SIGNAL(GraphMLItem_CenterAspects()), this, SLOT(fitToScreen()));

            if(nodeItem){
                connect(nodeItem, SIGNAL(NodeItem_MoveSelection(QPointF)), this, SLOT(moveSelection(QPointF)));
                connect(nodeItem, SIGNAL(NodeItem_ResizeSelection(QSizeF)), this, SLOT(resizeSelection(QSizeF)));
                connect(nodeItem, SIGNAL(NodeItem_SortModel()), this, SLOT(sortModel()));
                connect(nodeItem, SIGNAL(NodeItem_MoveFinished()), this, SLOT(moveFinished()));
                connect(nodeItem, SIGNAL(NodeItem_ResizeFinished()), this, SLOT(resizeFinished()));
                connect(this, SIGNAL(view_toggleGridLines(bool)), nodeItem, SLOT(toggleGridLines(bool)));

                connect(nodeItem, SIGNAL(Nodeitem_HasFocus(bool)), this, SLOT(editableItemHasFocus(bool)));

                connect(nodeItem, SIGNAL(NodeItem_showLockMenu(NodeItem*)), this, SLOT(showNodeItemLockMenu(NodeItem*)));
                //connect(nodeItem, SIGNAL(NodeItem_lockMenuClosed(NodeItem*)), this, SLOT(nodeItemLockMenuClosed(NodeItem*)));

            }
        }else{
            //Specific SubView Functionality.
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

        if(item){
            //disconnect(item, SIGNAL(GraphMLItem_SetGraphMLData(GraphMLItem*,QString,QString)), this, SLOT(setGraphMLData(GraphMLItem*,QString,QString)));
            if(scene()->items().contains(item)){

                // send necessary signals when a node has been destructed
                if (item->isNodeItem()) {
                    NodeItem* nodeItem = (NodeItem*) item;
                    nodeDestructed_signalUpdates(nodeItem);
                }

                // send necessary signals when an edge has been destructed
                if (item->isEdgeItem()) {
                    EdgeItem* edgeItem = (EdgeItem*) item;
                    Edge* edge = qobject_cast<Edge*>(edgeItem->getGraphML());
                    edgeDestructed_signalUpdates(edge, ID);
                }

                scene()->removeItem(item);
                delete item;
                //view_GraphMLItemDeleted(ID);


            }
            return true;
        }

        if(IS_SUB_VIEW){
            if(CENTRALIZED_ON_ITEM && centralizedItemID == ID){
                //CALL DELETE ON DIALOG
                this->parent()->deleteLater();
            }
        }
    }else{
        if(!IS_SUB_VIEW){
            qCritical() << "Could not find GraphMLItem from Hash!" << ID;
        }
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
    updateGoToActionsEnabled(getSelectedNode());
    emit view_nodeConstructed(nodeItem);

    // send specific current view states to the newly constaructed node item
    emit view_AspectsChanged(currentAspects);
    emit view_toggleGridLines(GRID_LINES_ON);

    // snap node item to its parent's grid
    nodeItem->snapToGrid();

    // initially hide all ManagementComponents
    if (nodeItem->getNodeKind() == "ManagementComponent") {
        nodeItem->setHidden(!managementComponentVisible);
    }

    // hide all AggregateInstances except for in OutEventPortImpls
    if (nodeItem->getNodeKind() == "AggregateInstance") {
        NodeItem* parentItem = nodeItem->getParentNodeItem();
        if (parentItem && (parentItem->getNodeKind() != "OutEventPortImpl"
                           && parentItem->getNodeKind() != "AggregateInstance"
                           && parentItem->getNodeKind() != "Aggregate"))
        {
            nodeItem->setHidden(true);
        }
    }
}


/**
 * @brief NodeView::nodeDestructed_signalUpdates
 * This gets called whenever a node has been destructed.
 * It sends signals to update whatever needs updating.
 * @param nodeItem
 */
void NodeView::nodeDestructed_signalUpdates(NodeItem* nodeItem)
{
    // update the docks and the toolbar/menu goTo functions
    updateGoToActionsEnabled(getSelectedNode());
    emit view_nodeDestructed(nodeItem);
}


/**
 * @brief NodeView::nodeSelected_signalUpdates
 * This gets called whenever a node has been selected.
 * It sends signals to update whatever needs updating.
 * @param node
 */
void NodeView::nodeSelected_signalUpdates(Node* node)
{
    if (selectedIDs.count() == 1) {
        // update the toolbar/menu goTo functions
        updateGoToActionsEnabled(node);
    } else {
        // disable the toolbar/menu goTo functions
        updateGoToActionsEnabled();
    }

    // update the docks regardless of the number of items selected
    emit view_nodeSelected();

    //keepSelectionFullyVisible();
}


/**
 * @brief NodeView::edgeConstructed_signalUpdates
 * This gets called whenever an edge is constructed.
 * It sends signals to update whatever needs updating.
 * @param edge
 */
void NodeView::edgeConstructed_signalUpdates(Edge* edge)
{
    // update the docks and the toolbar/menu goTo functions
    updateGoToActionsEnabled(getSelectedNode());
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
    // update the docks and the toolbar/menu goTo functions
    updateGoToActionsEnabled(getSelectedNode());
    emit view_edgeDestructed();

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
 * @brief NodeView::updateGoToActionsEnabled
 * This method updates the enabled state of the MEDEA window's menu actions
 * and the toolbar's tool buttons for the goToDefinition/goToImplementation
 * functions based on the currently selected node.
 * @param selectedNode
 */
void NodeView::updateGoToActionsEnabled(Node* selectedNode)
{
    Node* hasDefn = 0;
    Node* hasImpl = 0;

    if (selectedNode) {
        hasDefn = hasDefinition(selectedNode);
        hasImpl = hasImplementation(selectedNode);
    }

    // update goto toolbar buttons
    toolbar->showDefinitionButton(hasDefn);
    toolbar->showImplementationButton(hasImpl);

    // update goto menu actions
    emit view_updateGoToMenuActions("definition", hasDefn);
    emit view_updateGoToMenuActions("implementation", hasImpl);
}


/**
 * @brief NodeView::hasDefinition
 * This returns the selected node's definition if it has one.
 * @param node
 * @return
 */
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


/**
 * @brief NodeView::hasImplementation
 * This returns the selected node's implementation if it has one.
 * @param node
 * @return
 */
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
    if(RUBBERBAND_MODE && drawingRubberBand){
        //Get the Top Left and Bottom Right corners of the Rectangle.
        QPoint screenOrigin = rubberBand->pos();
        QPoint screenFinish = screenOrigin + QPoint(rubberBand->width(), rubberBand->height());
        QPointF sceneOrigin = mapToScene(screenOrigin);
        QPointF sceneFinish = mapToScene(screenFinish);
        selectedInRubberBand(sceneOrigin, sceneFinish);
        setRubberBandMode(false);
        return;
    }

    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* itemUnderMouse = scene()->itemAt(scenePos, QTransform());
    if (!itemUnderMouse) {
        if (event->button() == Qt::MiddleButton) {
            // center on the current view aspects
            fitToScreen();
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}


/**
 * @brief NodeView::mouseMoveEvent
 * @param event
 */
void NodeView::mouseMoveEvent(QMouseEvent *event)
{
    if(RUBBERBAND_MODE && drawingRubberBand){
        //Move rubberband to the position on the screen.
        rubberBand->setGeometry(QRect(rubberBandOrigin, event->pos()).normalized());
        if(!rubberBand->isVisible()){
            rubberBand->setVisible(true);
        }
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
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

    //If we have the Rubberband mode on, set the origin.
    if(RUBBERBAND_MODE){
        rubberBandOrigin = event->pos();
        //Move rubberband to the position on the screen.
        rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
        drawingRubberBand = true;
    }

    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* itemUnderMouse = scene()->itemAt(scenePos, QTransform());
    if (!itemUnderMouse) {
        if (event->button() == Qt::LeftButton) {
            // clear the selection and disable dock buttons
            clearSelection();
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
    QRectF viewRect = viewport()->rect();
    QRectF scaledSceneRect(QPointF(0,0), sceneRect().size()*transform().m11());

    if (event->delta() > 0) {
        // zoom in - maximum scale is when the scene is 50 times the size of the view
        if (viewRect.width()*MAX_ZOOM_RATIO > scaledSceneRect.width()) {
            scale(ZOOM_SCALE_INCREMENTOR, ZOOM_SCALE_INCREMENTOR);
        }
    } else if (event->delta() < 0) {
        // zoom out - minimum scale is when the view is twice the size of the scene
        if (viewRect.width() < scaledSceneRect.width()*MIN_ZOOM_RATIO) {
            scale(ZOOM_SCALE_DECREMENTOR, ZOOM_SCALE_DECREMENTOR);
        }
    }
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

    if(IS_MOVING){
        //FINALIZE MOVE
        moveFinished();
    }
    if(IS_RESIZING){
        resizeFinished();
    }



    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(CONTROL && SHIFT){
        if(!RUBBERBAND_MODE){
            setRubberBandMode(true);
        }
    }else{
        if(RUBBERBAND_MODE){
            setRubberBandMode(false);
        }
    }

    if(this->hasFocus()){

        if(CONTROL && event->key() == Qt::Key_A){
            selectAll();
        }

        if(CONTROL && event->key() == Qt::Key_D){
            duplicate();
        }

        // Added this to clear selection
        if (event->key() == Qt::Key_Escape){
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
    alignSelectionOnGrid(HORIZONTAL);
}


/**
 * @brief NodeView::alignSelectionVertically
 * This is called when the alignVertical button on the toolbar is clicked.
 * It aligns the selected items vertically.
 */
void NodeView::alignSelectionVertically()
{
    alignSelectionOnGrid(VERTICAL);
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
                showDialogMessage(WARNING, "Selection Issue", "Cannot Align Selection which aren't contained by the same Parent.", graphMLItem->getGraphML(), true);
                return;
            }

            averageX += nodeItem->centerPos().x();
            averageY += nodeItem->centerPos().y();
            itemCount++;
        }
    }

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
            nodeItem->setLocked(true);
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
    QGraphicsView::setEnabled(enabled);
}


void NodeView::showDialogMessage(MESSAGE_TYPE type, QString title, QString message, GraphML *item, bool centralizeItem)
{

    if(item && centralizeItem){
        centerItem(getGraphMLItemFromGraphML(item));
    }
    if(message != ""){
        if(type == CRITICAL){
            QMessageBox::critical(this, "Error: " + title, message, QMessageBox::Ok);
        }else if(type == WARNING){
            QMessageBox::warning(this, "Warning: " + title, message, QMessageBox::Ok);
        }else{
            QMessageBox::information(this, "Message: " + title, message, QMessageBox::Ok);
        }
    }
}


void NodeView::duplicate()
{
    view_Duplicate(selectedIDs);
}

void NodeView::copy()
{
    view_Copy(selectedIDs);
}

void NodeView::cut()
{
    view_Cut(selectedIDs);
}

void NodeView::paste(QString xmlData)
{
    Node* selectedNode = this->getSelectedNode();
    view_Paste(selectedNode, xmlData);
}

void NodeView::selectAll()
{
    NodeItem* nodeItem = getSelectedNodeItem();
    if(nodeItem){
        clearSelection(false);
        foreach(NodeItem* child, nodeItem->getChildNodeItems()){
            if(child->isVisible()){
                appendToSelection(child);
            }
        }
    }
}


/**
 * @brief NodeView::undo
 * This undoes the previous triggered action and recenters the
 * view on the same spot when that action was triggered.
 */
void NodeView::undo()
{
    if (controller) {

        /*
        // center the view on the corresponding pos/rect
        if (!viewModelPositions.isEmpty() && !viewCenteredRectangles.isEmpty() ) {
            QRectF rect = viewCenteredRectangles.takeFirst();
            QPointF pos = viewModelPositions.takeFirst();
            recenterView(pos, rect, true);
        }
        */

        // keep the view centered on the same spot
        if (getMapSize() > 0) {
            QPointF pos = modelPositions[getMapSize() - 1];
            QRectF rect = centeredRects[getMapSize() - 1];
            recenterView(pos, rect, true);
        }

        // undo the action
        controller->undo();
    }
}


/**
 * @brief NodeView::redo
 * This redoes the previous undo action and recenters the
 * view on the same spot when that action was triggered.
 */
void NodeView::redo()
{
    if (controller) {

        /*
        // center the view on the corresponding pos/rect
        if (!viewModelPositions.isEmpty() && !viewCenteredRectangles.isEmpty() ) {
            QRectF rect = viewCenteredRectangles.takeFirst();
            QPointF pos = viewModelPositions.takeFirst();
            recenterView(pos, rect, true);
        }
        */

        // keep the view centered on the same spot
        if (getMapSize() > 0) {
            QPointF pos = modelPositions[getMapSize() - 1];
            QRectF rect = centeredRects[getMapSize() - 1];
            recenterView(pos, rect, true);
        }

        // redo the action
        controller->redo();
    }
}


void NodeView::appendToSelection(GraphMLItem *item)
{
    if(isItemsAncestorSelected(item)){
        return;
    }

    //qDebug() << "appendToSelection: " << item->sceneBoundingRect().bottom();

    //Unset Items Descendant Items.
    unsetItemsDescendants(item);

    //Set this item as Selected.
    setGraphMLItemSelected(item, true);

    //fitSelectionInView();
}

void NodeView::removeFromSelection(GraphMLItem *item)
{
    if(!isItemsAncestorSelected(item)){
        return;
    }
    //Set this item as Selected.
    setGraphMLItemSelected(item, false);


}

void NodeView::moveSelection(QPointF delta)
{
    bool canReduceX = true;
    bool canReduceY = true;

    foreach(QString ID, selectedIDs){

        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);

        GraphML* graphml = graphMLItem->getGraphML();
        if(graphml && graphml->isNode()){

            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            QPointF resultingPosition = nodeItem->pos() + delta;

            // added this so that the Definitions containers can be moved outside of the Model
            if (nodeItem->getNodeKind().endsWith("Definitions")) {
                canReduceX = true;
                canReduceY = true;
                continue;
            }

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
    foreach(QString ID, selectedIDs){
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);


        GraphML* graphml = graphMLItem->getGraphML();
        if(graphml && graphml->isNode()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            nodeItem->setNodeMoving(true);
            nodeItem->adjustPos(delta);
            IS_MOVING = true;
        }
    }
}

void NodeView::resizeSelection(QSizeF delta)
{
    if(!getSelectedNode()){
        return;
    }

    foreach(QString ID, selectedIDs){
        IS_RESIZING = true;
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
        GraphML* graphml = graphMLItem->getGraphML();
        if(graphml && graphml->isNode()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            nodeItem->setNodeResizing(true);
            nodeItem->adjustSize(delta);
        }
    }

}

void NodeView::moveFinished()
{
    IS_MOVING = false;

    foreach(QString ID, selectedIDs){
        GraphMLItem* currentItem = getGraphMLItemFromHash(ID);
        if(currentItem && currentItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) currentItem;
            nodeItem->setNodeMoving(false);
            nodeItem->updateModelPosition();
        }
    }
}

void NodeView::resizeFinished()
{

    foreach(QString ID, selectedIDs){
        GraphMLItem* currentItem = getGraphMLItemFromHash(ID);
        if(currentItem && currentItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) currentItem;
            nodeItem->setNodeResizing(false);
            nodeItem->updateModelSize();
        }
        IS_RESIZING = false;
    }
}


void NodeView::view_ClearHistory()
{
    view_ClearHistoryStates();
    viewCenterPointStack.clear();

    // clear the maps used for the moving the view backwards & forwards
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
        view_SetAttributeModel(0);
    }

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

    clearSelection();

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


void NodeView::destructGUIItem(QString ID)
{
    removeGraphMLItemFromHash(ID);
}


/**
 * @brief NodeView::showManagementComponents
 * @param show
 */
void NodeView::showManagementComponents(bool show)
{
    QList<Node*> managementComponents;
    Model* model = controller->getModel();
    if (model) {
        managementComponents = model->getChildrenOfKind("ManagementComponent");
    }

    // this goes through all the ManagementComponents and shows/hides them
    foreach (Node* node, managementComponents) {
        NodeItem* nodeItem = getNodeItemFromNode(node);
        if (nodeItem) {
            nodeItem->setHidden(!show);
            if (show) {
                // make sure that the aspects for Deployment are turned on
                viewDeploymentAspect();
                nodeItem->snapToGrid(); // sort here instead?
            }
        }
    }

    managementComponentVisible = show;
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
    QRectF itemsRec = scene()->itemsBoundingRect();
    float leftMostX = itemsRec.bottomRight().x();
    float rightMostX = itemsRec.topLeft().x();
    float topMostY = itemsRec.bottomRight().y();
    float bottomMostY = itemsRec.topLeft().y();

    // if there is no list provided, use the full node items list
    if (itemsToCenter.isEmpty()) {
        itemsToCenter = getNodeItemsList();
    }

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

    QRectF visibleItemsRec = QRectF(leftMostX, topMostY, abs((rightMostX-leftMostX)), abs((bottomMostY-topMostY)));
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

    Node* defn = hasDefinition(node);

    if (defn) {
        // make sure the Definitions view aspect is on
        addAspect("Definitions");

        GraphMLItem* guiItem = getGraphMLItemFromGraphML(defn);
        clearSelection(false);
        appendToSelection(guiItem);
        centerOnItem();
    }
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

    Node* impl = hasImplementation(node);

    if (impl) {
        // make sure the Workload view aspect is on
        addAspect("Workload");

        GraphMLItem* guiItem = getGraphMLItemFromGraphML(impl);
        clearSelection(false);
        appendToSelection(guiItem);
        centerOnItem();
    }
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
    triggerAction("Toolbar: Destructing Selection");
    view_SetAttributeModel(0);
    view_Delete(selectedIDs);
}


/**
 * @brief NodeView::resetModel
 * This method is called after the model is cleared.
 * It resets the size, sorts and centers the model.
 */
void NodeView::resetModel()
{
    triggerAction("Resetting Model");
    foreach (NodeItem* nodeItem, getNodeItemsList()) {
        if (nodeItem) {
            nodeItem->resetSize();
        }
    }

    sortModel();
    setAspects(defaultAspects);
}


/**
 * @brief NodeView::clearModel
 * This tells the controller to clear the model.
 * This is called from MedeaWindow.
 */
void NodeView::clearModel()
{
    if (controller) {
        controller->clearModel();
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
    if (getModelItem()) {
        getModelItem()->newSort();
    }
}
