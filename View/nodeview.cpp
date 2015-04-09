#include "nodeview.h"
#include "../Controller/controller.h"
#include "toolbar/toolbarwidget.h"
#include "dock/docktogglebutton.h"
#include <limits>

//Qt includes
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

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR
#define MIN_ZOOM 0.01
#define MAX_ZOOM 0.5

NodeView::NodeView(bool subView, QWidget *parent):QGraphicsView(parent)
{
    constructedFromImport = true;
    CENTRALIZED_ON_ITEM = false;
    IS_SUB_VIEW = subView;
    controller = 0;
    toolbarJustClosed = false;
    parentNodeView = 0;
    rubberBand = 0;
    CONTROL_DOWN = false;
    SHIFT_DOWN = false;

    setScene(new QGraphicsScene(this));

    //Set QT Options for this QGraphicsView
    setDragMode(ScrollHandDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //Set GraphicsView background-color
    setStyleSheet("QGraphicsView{ background-color: rgba(175,175,175,255); border: 0px;}");

    //QBrush brush( QColor(100,100,200,250) );
    //scene()->setBackgroundBrush(brush);

    //Set The rubberband Mode.
    setRubberBandMode(false);

    //Setup Aspects
    allAspects << "Assembly";
    allAspects << "Hardware";
    allAspects << "Definitions";
    allAspects << "Workload";

    defaultAspects << "Definitions";

    nonDrawnItemKinds << "DeploymentDefinitions";

    //create toolbar widget
    toolbar = new ToolbarWidget(this);

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

bool NodeView::isAspectVisible(QString aspect)
{
    return currentAspects.contains(aspect);
}

void NodeView::addAspect(QString aspect)
{
    if(!isAspectVisible(aspect)){
        QStringList newAspects = currentAspects;
        newAspects.append(aspect);
        setAspects(newAspects);
    }
}

void NodeView::removeAspect(QString aspect)
{
    if(isAspectVisible(aspect)){
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
 * Get the current rectangle visualised by this NodeView
 * @return
 */
QRectF NodeView::getVisibleRect( )
{
    QPointF topLeft = mapToScene(0,0);
    QPointF bottomRight = mapToScene(viewport()->width(),viewport()->height());
    return QRectF( topLeft, bottomRight );
}


/**
 * @brief NodeView::adjustSceneRect
 * This makes the necessary adjustments to the sceneRect to be able to center on rectToCenter.
 * @param rectToCenter
 */
void NodeView::adjustSceneRect(QRectF rectToCenter)
{
    QRectF viewRect = getVisibleRect();
    QRectF newRec = scene()->itemsBoundingRect();

    qreal multiplier = (viewRect.height() / rectToCenter.height()) - 1;
    qreal neededXGap = qAbs((viewRect.width() - (rectToCenter.width()*multiplier)) / 2);
    qreal neededYGap = qAbs((viewRect.height() - (rectToCenter.height()*multiplier)) / 2);
    qreal leftXGap = qAbs((rectToCenter.x() - sceneRect().x()) * multiplier);
    qreal rightXGap = qAbs((sceneRect().x() + sceneRect().width() - (rectToCenter.x() + rectToCenter.width()))  * multiplier);
    qreal topYGap = qAbs((rectToCenter.y() - sceneRect().y()) * multiplier);
    qreal bottomYGap = qAbs(((sceneRect().y() + sceneRect().height()) - (rectToCenter.y() + rectToCenter.height())) * multiplier);

    // check to make sure that there is enough space around the items boundingRect within
    // the scene before centering it; if there isn't, add the needed space to the sceneRect
    if (leftXGap < neededXGap && neededXGap > 0) {

        newRec.setX(newRec.x()-neededXGap);
        newRec.setWidth(newRec.width()+neededXGap);
        setSceneRect(newRec);
    } else if (rightXGap < neededXGap && neededXGap > 0 ) {
        newRec.setWidth(newRec.width()+neededXGap);
        setSceneRect(newRec);
    }
    if (topYGap < neededYGap && neededYGap > 0) {
        newRec.setY(newRec.y()-neededYGap);
        newRec.setHeight(newRec.height()+neededYGap);
        setSceneRect(newRec);
    } else if (bottomYGap < neededYGap && neededYGap > 0) {
        newRec.setHeight(newRec.height()+neededYGap);
        setSceneRect(newRec);
    }
}


/**
 * @brief NodeView::centerRect
 * This centers the provided rectangle in the view.
 * @param rect
 */
void NodeView::centerRect(QRectF rect, float extraspace)
{
    QPointF rectCenter = rect.center();
    float extraSpace = 1.25;

    // check if there is a specified value for extraspace
    if (extraspace > 0) {
        extraSpace = extraspace;
    }

    // add extra space to the visible itemsBoundingRect to create a margin in the view
    rect.setWidth(rect.width()*extraSpace*extraSpace);
    rect.setHeight(rect.height()*extraSpace);

    // adjust the sceneRect to be able to center on rect, then fit and center rect in view
    adjustSceneRect(rect);
    fitInView(rect, Qt::KeepAspectRatio);
    centerOn(rectCenter);
}


/**
 * @brief NodeView::getNodeItemsList
 * Return the list of all node items on the scene.
 * @return
 */
QList<NodeItem*> NodeView::getNodeItemsList()
{
    QList<NodeItem*> nodeItems;

    foreach(GraphMLItem* item, guiItems){
        if(item->isNodeItem()){
            nodeItems.append((NodeItem*)item);
        }
    }

    return nodeItems;
}


/**
 * @brief NodeView::showAllViewAspects
 */
void NodeView::showAllAspects()
{
    view_AspectsChanged(allAspects);
}


/**
 * @brief NodeView::getSelectedNode
 * @return
 */
Node *NodeView::getSelectedNode()
{
    if(selectedIDs.size() == 1){
        QString ID = selectedIDs[0];
        GraphMLItem* item = getGraphMLItemFromHash(ID);
        if(item->isNodeItem()){
            return (Node*)item->getGraphML();
        }
    }

    return 0;
}


/**
 * @brief NodeView::getSelectedNodeItem
 * @return
 */
NodeItem *NodeView::getSelectedNodeItem()
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
        subViews.removeAll(subView);
    }
}

QStringList NodeView::getAdoptableNodeList(Node *node)
{
    return controller->getAdoptableNodeKinds(node);
}

QList<Node *> NodeView::getConnectableNodes(Node *node)
{
    return controller->getConnectableNodes(node);
}


void NodeView::constructNewView(Node *centeredOn)
{
    if(IS_SUB_VIEW){
        //Cannot make subviews of subviews.
        return;
    }
    //centeredOn = getSelectedNode();
    if(!centeredOn){
        return;
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

/**
 * @brief NodeView::sortEntireModel
 * This method sorts the enitre model.
 */
void NodeView::sortEntireModel()
{
    sortNode(controller->getModel());
}


/**
 * @brief NodeView::sortNode
 * This method recursively sorts the provided node.
 * It sorts from the lowest level children back up to the provided node.
 * @param node
 */
void NodeView::sortNode(Node *node, Node* topMostNode)
{
    if (!topMostNode) {
        emit view_TriggerAction("View: Sorting Node");
        topMostNode = node;
    }

    GraphMLItem* graphMLItem = getGraphMLItemFromGraphML(node);
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(graphMLItem);

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


bool NodeView::viewportEvent(QEvent * e)
{
    emit view_ViewportRectChanged(getVisibleRect());
    return QGraphicsView::viewportEvent(e);
}


/**
 * @brief NodeView::centreItem
 * This method scales and translates the scene to center on the item.
 * It extends the scene rect if it's not big enough to centre the item.
 * @param item
 */
void NodeView::centerItem(GraphMLItem *item)
{
    if (!item) {
        qCritical() << "No GUI item to Center";
        return;
    } else {
        //qDebug() << "Centering item: " << item->getGraphML()->getDataValue("kind");
    }

    QRectF itemRect = ((QGraphicsItem*)item)->sceneBoundingRect();
    centerRect(itemRect);
}


/**
 * @brief NodeView::clearView
 */
void NodeView::clearView()
{
    scene()->clear();
    viewport()->update();
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
 * @brief NodeView::setViewAspects
 * This is called whenever the view aspect buttons are clicked.
 * @param aspects
 */
void NodeView::setAspects(QStringList aspects)
{
    currentAspects = aspects;

    if(!IS_SUB_VIEW && AUTO_CENTER_ASPECTS){
        centerAspects();
    }

    view_AspectsChanged(aspects); // need this for all cases
    view_GUIAspectChanged(aspects);

    // TODO: only need to clear the selection if the selected item is now hidden
    clearSelection();
}


/**
 * @brief NodeView::centerOnItem
 * This should center on the selected node without changing the view's scale.
 * At the moment, this only works upto a certain scale.
 */
void NodeView::centerOnItem()
{
    if (getSelectedNode()) {
        NodeItem* selectedItem = getNodeItemFromNode(getSelectedNode());
        QRectF itemRect = selectedItem->sceneBoundingRect();
        adjustSceneRect(itemRect);
        centerOn(itemRect.center());
        /*
        qreal prevScale = transform().m11();
        centerItem(selectedItem);
        scale(prevScale/transform().m11(), prevScale/transform().m11());
        */
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

    // when a painted nodeitem is selected, show context toolbar
    NodeItem* nodeItem = getNodeItemFromNode(getSelectedNode());
    if (nodeItem && nodeItem->isPainted() && nodeItem->sceneBoundingRect().contains(toolbarPosition)) {
        // update toolbar position and connect selected node item
        toolbar->setNodeItem(nodeItem);
        toolbar->move(globalPos);
        toolbar->show();
    } else {
        // show/hide MEDEA toolbar
        view_showWindowToolbar();
    }
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

    NodeItem* nodeItem = new NodeItem(node, parentNodeItem, currentAspects, IS_SUB_VIEW);

    storeGraphMLItemInHash(nodeItem);

    //Connect the Generic Functionality.
    connectGraphMLItemToController(nodeItem, node);

    if(scene() && !scene()->items().contains(nodeItem)){
        //Add to model.
        scene()->addItem(nodeItem);
    }

    /*
    // select hardware clusters on construction
    if (nodeItem->getNodeKind() == "HardwareCluster") {
        //clearSelection(true, false);
        nodeItem->setSelected(true);
        appendToSelection(nodeItem);
    }
    */

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

        /******************************************************************/
        // show hidden hardware node before the new edge is added to scene
        if (dstGUI->getGraphML()->getDataValue("kind") == "HardwareNode") {
            dstGUI->setHidden(false);
        }


        if(!scene()->items().contains(nodeEdge)){
            //Add to model.
            scene()->addItem(nodeEdge);
        }


    }else{
        if(!IS_SUB_VIEW){
            qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
        }
    }
}



void NodeView::view_SortNode(Node *node)
{
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(node->getID()));
    if(nodeItem){
        //nodeItem->sort();
        nodeItem->newSort();
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

void NodeView::view_SetOpacity(GraphML *graphML, qreal opacity)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    if(guiItem){
        guiItem->setOpacity(opacity);
    }
}

/**
 * @brief NodeView::constructNode
 * @param nodeKind - kind of node to construct
 * @param sender - 0 = DockScrollArea, 1 = ToolbarWidget
 */
void NodeView::constructNode(QString nodeKind, int sender)
{
    view_TriggerAction("Dock/Toolbar: Constructing Node");

    NodeItem* selectedItem = getNodeItemFromNode(getSelectedNode());
    if (selectedItem) {
        constructedFromImport = false;
        if (sender == 0) {
            view_ConstructNode(getSelectedNode(), nodeKind, selectedItem->getNextChildPos());
        } else if (sender == 1) {
            view_ConstructNode(getSelectedNode(), nodeKind, selectedItem->mapFromScene(toolbarPosition));
        }
    }
}


/**
 * @brief NodeView::view_ConstructEdgeAction
 * @param src
 * @param dst
 */
void NodeView::constructEdge(Node *src, Node *dst)
{
    view_TriggerAction("Dock/Toolbar: Constructing Edge");
    view_ConstructEdge(src, dst);

    // send necessary signals when an edge has been constucted
    edgeConstructed_signalUpdates(src);
}


/**
 * @brief NodeView::view_addComponentInstance
 * @param assm - ComponentAssembly (selected node)
 * @param defn - Component from selected dock/toolbar item
 * @param sender - 0 = DockScrollArea, 1 = ToolbarWidget
 */
void NodeView::constructComponentInstance(Node* assm, Node* defn, int sender)
{
    emit view_TriggerAction("Dock/Toolbar: Constructing ComponentInstance");

    ComponentAssembly* assembly = dynamic_cast<ComponentAssembly*>(assm);
    Component* definition = dynamic_cast<Component*>(defn);

    if (assembly && definition) {
        GraphMLItem *graphMLItem = getGraphMLItemFromGraphML(assembly);
        if (sender == 0) {
            NodeItem *nodeItem = getNodeItemFromGraphMLItem(graphMLItem);
            view_ConstructComponentInstance(assembly, definition, nodeItem->getNextChildPos());
        } else if (sender == 1) {
            view_ConstructComponentInstance(assembly, definition, graphMLItem->mapFromScene(toolbarPosition));
        }

    }
}


/**
 * @brief NodeView::constructConnectedNode
 * @param parentNode
 * @param node
 * @param kind
 * @param sender
 */
void NodeView::constructConnectedNode(Node *parentNode, Node *node, QString kind, int sender)
{
    view_TriggerAction("Dock/Toolbar: Constructing ComponentInstance");

    if (parentNode && node) {
        GraphMLItem *graphMLItem = getGraphMLItemFromGraphML(parentNode);
        if (sender == 0) {

            NodeItem *nodeItem = getNodeItemFromGraphMLItem(graphMLItem);
            //qCrutucal() << nodeItem->getGraphML()->toString();
            view_ConstructConnectedComponents(parentNode, node, kind, nodeItem->getNextChildPos());
        } else if (sender == 1) {
            //qCrutucal() << nodeItem->getGraphML()->toString();
            view_ConstructConnectedComponents(parentNode, node, kind, graphMLItem->mapFromScene(toolbarPosition));
        }
    }
}


/**
 * @brief NodeView::view_constructEventPortDelegate
 * @param assm
 * @param eventPortInstance
 */
void NodeView::constructEventPortDelegate(Node *assm, Node *eventPortInstance)
{
    view_TriggerAction("Toolbar: Constructing EventPortDelegate");

    ComponentAssembly* assembly = dynamic_cast<ComponentAssembly*>(assm);
    if (assembly) {
        GraphMLItem *graphMLItem = getGraphMLItemFromGraphML(assembly);
        QString portKind = eventPortInstance->getDataValue("kind") + "Delegate";
        view_ConstructConnectedComponents(assm, eventPortInstance,portKind, graphMLItem->mapFromScene(toolbarPosition));
    }
}



/**
 * @brief NodeView::componentInstanceConstructed
 * This is called when a ComponentInstance is created with a definition.
 * It forces an update on its parent's height after inheriting graphML values from its definition.
 * @param node
 */
void NodeView::componentInstanceConstructed(Node *node)
{
    GraphMLItem* graphMLItem = getGraphMLItemFromGraphML(node);
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(graphMLItem);
    if (nodeItem) {

    }
}


/**
 * @brief NodeView::fitInSceneRect
 */
void NodeView::fitInSceneRect(GraphMLItem* item)
{
    adjustSceneRect(item->sceneBoundingRect());
}


void NodeView::setGraphMLItemAsSelected(GraphMLItem *item)
{
    QString itemID = item->getGraphML()->getID();

    if(!selectedIDs.contains(itemID)){
        selectedIDs.append(itemID);
        item->setSelected(true);
        if(item->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) item;
            nodeSelected_signalUpdates(nodeItem->getNode());
        }
        emit view_SetAttributeModel(item->getAttributeTable());
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
 * @return
 */
QList<Node*> NodeView::getFiles()
{
    QList<Node*> returnList;
    Model* model = controller->getModel();
    if (model) {
        returnList = model->getChildrenOfKind("File");
    }
    return returnList;
}


/**
 * @brief NodeView::getComponents
 * @return
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


/**
 * @brief NodeView::appendToSelection
 * @param node
 */
void NodeView::appendToSelection(Node *node)
{
   appendToSelection(getGraphMLItemFromGraphML(node));
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
            connect(GUIItem, SIGNAL(GraphMLItem_SetCentered(GraphMLItem*)), this, SLOT(centerItem(GraphMLItem*)));
            connect(GUIItem, SIGNAL(GraphMLItem_MovedOutOfScene(GraphMLItem*)), this, SLOT(fitInSceneRect(GraphMLItem*)));
            connect(this, SIGNAL(view_AspectsChanged(QStringList)), GUIItem, SLOT(aspectsChanged(QStringList)));
        }

        if(!IS_SUB_VIEW){
            connect(GUIItem, SIGNAL(GraphMLItem_TriggerAction(QString)),  this, SIGNAL(view_TriggerAction(QString)));

            connect(GUIItem, SIGNAL(GraphMLItem_SetGraphMLData(GraphML*,QString,QString)), this, SIGNAL(view_SetGraphMLData(GraphML*,QString,QString)));
            connect(GUIItem, SIGNAL(GraphMLItem_ConstructGraphMLData(GraphML*,QString)), this, SIGNAL(view_ConstructGraphMLData(GraphML*,QString)));
            connect(GUIItem, SIGNAL(GraphMLItem_DestructGraphMLData(GraphML*,QString)), this, SIGNAL(view_DestructGraphMLData(GraphML*,QString)));

            connect(GUIItem, SIGNAL(GraphMLItem_CenterAspects()), this, SLOT(centerAspects()));

            if(nodeItem){
                connect(nodeItem, SIGNAL(NodeItem_MoveSelection(QPointF)), this, SLOT(moveSelection(QPointF)));
                connect(nodeItem, SIGNAL(NodeItem_ResizeSelection(QSizeF)), this, SLOT(resizeSelection(QSizeF)));
                connect(nodeItem, SIGNAL(NodeItem_SortModel()), this, SLOT(sortModel()));
                connect(nodeItem, SIGNAL(NodeItem_MoveFinished()), this, SLOT(moveFinished()));
                connect(nodeItem, SIGNAL(NodeItem_ResizeFinished()), this, SLOT(resizeFinished()));
                connect(this, SIGNAL(view_toggleGridLines(bool)), nodeItem, SLOT(toggleGridLines(bool)));
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
                if(item->isNodeItem()){
                    NodeItem* nodeItem = (NodeItem*) item;
                    nodeDestructed_signalUpdates(nodeItem);
                }
                scene()->removeItem(item);
                delete item;
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

void NodeView::nodeSelected_signalUpdates(Node *node)
{
    if (node) {

        Node* hasDefn = hasDefinition(node);
        Node* hasImpl = hasImplementation(node);

        // update goto menu actions
        emit view_updateGoToMenuActions("definition", hasDefn);
        emit view_updateGoToMenuActions("implementation", hasImpl);

        // update goto toolbar buttons
        toolbar->showDefinitionButton(hasDefn);
        toolbar->showImplementationButton(hasImpl);

        emit view_nodeSelected(getSelectedNode());
    }
}


/**
 * @brief NodeView::nodeDestructed_signalUpdates
 * This gets called whenever a node has been destructed.
 * It sends signals to update whatever needs updating.
 * @param node
 */
void NodeView::nodeDestructed_signalUpdates(NodeItem* nodeItem)
{
    emit view_nodeSelected(0);
    emit view_nodeDestructed(nodeItem);
}


/**
 * @brief NodeView::nodeConstructed_signalUpdates
 * This is called whenever a node is constructed.
 * It sends signals to update whatever needs updating.
 * @param node
 */void NodeView::nodeConstructed_signalUpdates(NodeItem *nodeItem)
{
    emit view_nodeConstructed(nodeItem);
    nodeItem->toggleGridLines(GRID_LINES_ON);
}


/**
 * @brief NodeView::edgeConstructed_signalUpdates
 * This gets called whenever an edge is constructed.
 * It sends signals to update whatever needs updating.
 * @param src
 */
void NodeView::edgeConstructed_signalUpdates(Node *src)
{
    Node* hasDefn = hasDefinition(src);
    Node* hasImpl = hasImplementation(src);

    // update specific tool buttons when a new edge is constructed
    toolbar->showDefinitionButton(hasDefn);
    toolbar->showImplementationButton(hasImpl);

    // update node goto menu actions
    emit view_updateGoToMenuActions("definition", hasDefn);
    emit view_updateGoToMenuActions("implementation", hasImpl);
}

/**
 * @brief NodeView::hasDefinition
 * This returns the selected node's definition if it has one.
 * @param node
 * @return
 */
Node* NodeView::hasDefinition(Node *node)
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
Node* NodeView::hasImplementation(Node *node)
{
    Node* original = node;
    node = hasDefinition(node);
    if (!node) {
        node = original;
    }
    //Top Most Node.
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

    if(!itemUnderMouse){
        if(event->button() == Qt::MiddleButton){
            //Sort and center current view aspects
            centerAspects();
        }else{
            //Clear selection and disable dock buttons
            clearSelection();
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}

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


void NodeView::mousePressEvent(QMouseEvent *event)
{   
    if(toolbarJustClosed){
        toolbarJustClosed = false;
        return;
    }

    //If we have the Rubberband mode on, set the origin.
    if(RUBBERBAND_MODE){
        rubberBandOrigin = event->pos();
        //Move rubberband to the position on the screen.
        rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
        drawingRubberBand = true;
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
    // Scale the view / do the zoom
    if(event->delta() > 0 && transform().m11() < MAX_ZOOM) {
        // Zoom in
        scale(ZOOM_SCALE_INCREMENTOR, ZOOM_SCALE_INCREMENTOR);
    } else if (event->delta() < 0 && transform().m11() > MIN_ZOOM) {
        // Zooming out
        scale(ZOOM_SCALE_DECREMENTOR, ZOOM_SCALE_DECREMENTOR);
    }
}


/**
 * @brief NodeView::mouseDoubleClickEvent
 * @param event
 */
void NodeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* itemUnderMouse = scene()->itemAt(scenePos, QTransform());

    // added this to center aspects when double-clicking on the view
    if(!itemUnderMouse && event->button() == Qt::LeftButton){
        centerAspects();
    }

    QGraphicsView::mouseDoubleClickEvent(event);
}


void NodeView::keyPressEvent(QKeyEvent *event)
{
    if(IS_MOVING){
        //FINALIZE MOVE
        moveFinished();
    }
    if(IS_RESIZING){
        resizeFinished();
    }

    if(this->hasFocus()){

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

void NodeView::keyReleaseEvent(QKeyEvent *event)
{
    if(this->hasFocus()){
        if(event->key() == Qt::Key_Delete){
            deleteSelection();
        }
    }

    QGraphicsView::keyReleaseEvent(event);

}

void NodeView::alignSelectionHorizontally()
{
    alignSelectionOnGrid(HORIZONTAL);
}

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
                showDialogMessage(WARNING, "Cannot Align Selection which aren't contained by the same Parent.", graphMLItem->getGraphML());
                return;
            }

            averageX += nodeItem->centerPos().x();
            averageY += nodeItem->centerPos().y();
            itemCount++;
        }
    }

    averageX /= itemCount;
    averageY /= itemCount;

    qCritical() <<"AverageX: "<< averageX;
    qCritical() <<"AverageY: "<< averageY;


    QPointF centerPoint;
    if(sharedParent){
        //Find closest Grid Line
        NodeItem* parent = (NodeItem*)sharedParent;
        centerPoint = parent->getClosestGridPoint(QPointF(averageX, averageY));
    }

    foreach(QString ID, selectedIDs){
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
        if(graphMLItem && graphMLItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            QPointF pos = nodeItem->centerPos();

            if(alignment == VERTICAL){
                pos.setX(centerPoint.x());
            }
            if(alignment == HORIZONTAL){
                pos.setY(centerPoint.y());
            }
            nodeItem->setCenterPos(pos);
        }
    }
}

void NodeView::snapSelectionToGrid()
{
    foreach(QString ID, selectedIDs){
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);
        if(graphMLItem && graphMLItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
            nodeItem->snapToGrid();
        }
    }
}

void NodeView::snapChildrenToGrid()
{
    NodeItem* currentSelected= getSelectedNodeItem();
    if(currentSelected){
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




void NodeView::showDialogMessage(MESSAGE_TYPE type, QString message, GraphML *item)
{

    if(item){
        centerItem(getGraphMLItemFromGraphML(item));
    }
    if(message != ""){
        if(type == CRITICAL){
            QMessageBox::critical(this, "Error", message, QMessageBox::Ok);
        }else if(type == WARNING){
            QMessageBox::warning(this, "Warning", message, QMessageBox::Ok);
        }else{
            QMessageBox::information(this, "Message", message, QMessageBox::Ok);
        }
    }
}



void NodeView::view_SelectModel()
{
    Model* model = controller->getModel();
    GraphMLItem* modelItem = getGraphMLItemFromGraphML(model);
    clearSelection(false);
    appendToSelection(modelItem);
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
    Node* node = this->getSelectedNode();
    if(node){
        NodeItem* nodeItem = getNodeItemFromNode(node);
        if(nodeItem){
            clearSelection(false);
            foreach(NodeItem* child, nodeItem->getChildNodeItems()){
                if(child->isVisible()){
                    appendToSelection(child);
                }
            }
        }
    }
}

void NodeView::appendToSelection(GraphMLItem *item)
{
    if(isItemsAncestorSelected(item)){
        return;
    }

    //Unset Items Descendant Items.
    unsetItemsDescendants(item);

    //Set this item as Selected.
    setGraphMLItemAsSelected(item);
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
            if(resultingPosition.y() < minY ){
                canReduceY = false;
            }
        }
    }

    if(!canReduceX){
        delta.setX(0);
    }

    if(!canReduceY){
        delta.setY(0);
    }
    foreach(QString ID, selectedIDs){
        GraphMLItem* graphMLItem = getGraphMLItemFromHash(ID);


        GraphML* graphml = graphMLItem->getGraphML();
        if(graphml && graphml->isNode()){
            NodeItem* nodeItem = (NodeItem*) graphMLItem;
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
            nodeItem->updateModelPosition();
        }
    }
    IS_MOVING = false;
}

void NodeView::resizeFinished()
{

    foreach(QString ID, selectedIDs){
        GraphMLItem* currentItem = getGraphMLItemFromHash(ID);
        if(currentItem && currentItem->isNodeItem()){
            NodeItem* nodeItem = (NodeItem*) currentItem;
            nodeItem->updateModelSize();
        }
        IS_RESIZING = false;
    }
}


/**
 * @brief NodeView::clearSelection
 * This gets called when either the view or the model is pressed.
 * It clears the selection.
 */
void NodeView::clearSelection(bool updateTable, bool fullClear)
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

    // this stops the docks/dock buttons from disabling when they don't need to
    // if the call came from a painted node item, then it's just to deselect nodes
    // in case the parent of the node you're trying to select is already selected
    NodeItem* senderItem = dynamic_cast<NodeItem*>(QObject::sender());
    if ((senderItem && senderItem->isPainted()) || !fullClear) {
        return;
    }

    // update docks
    emit view_nodeSelected(0);
}



void NodeView::view_ClearHistory()
{
    view_ClearHistoryStates();
}

void NodeView::toolbarClosed()
{
    toolbarJustClosed = true;
}



/**
 * @brief NodeView::sortAspects
 * This method forces all of the view aspects to be visible and then sorts them.
 * It then sets the view aspects back to how it was before this method was called.
 */
void NodeView::sortAspects()
{
    QStringList oldAspect = currentAspects;
    showAllAspects();
    sortModel();
    view_AspectsChanged(oldAspect);
}


/**
 * @brief NodeView::resetModel
 * This method is called after the model is cleared.
 * It resets the size, sorts and centers the model.
 */
void NodeView::resetModel()
{
    view_TriggerAction("Resetting Model");
    foreach(NodeItem* nodeItem, getNodeItemsList()){
        if (nodeItem) {
            nodeItem->resetSize();
        }
    }


    sortAspects();
    setAspects(defaultAspects);
}

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
        Node* node = currentNode->getNode();

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
 * @brief NodeView::fitToScreen
 * This makes sure that all the visible items fit and are centered within the view.
 */
void NodeView::fitToScreen()
{
    QRectF itemsRec = scene()->itemsBoundingRect();
    float leftMostX = itemsRec.bottomRight().x();
    float rightMostX = itemsRec.topLeft().x();
    float topMostY = itemsRec.bottomRight().y();
    float bottomMostY = itemsRec.topLeft().y();

    // go through each item and store the left/right/top/bottom most coordinates
    // of the visible items to create the visible itemsBoundingRect to center on
    foreach (QGraphicsItem* item, scene()->items(itemsRec)) {

        NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);
        if (nodeItem && nodeItem->isPainted()) {
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
    centerRect(visibleItemsRec);
}


/**
 * @brief NodeView::goToDefinition
 * If the node is a definition, select and center it.
 * If it's not but it has a definition, center on its definition.
 * @param node
 * @param show
 */
void NodeView::goToDefinition(Node *node)
{
    if (!node) {
        node = getSelectedNode();
    }
    if (node) {
        Node* defn = hasDefinition(node);
        if (defn) {
            // make sure the Definitions view aspect is on
            addAspect("Definitions");
            GraphMLItem* guiItem = getGraphMLItemFromGraphML(defn);
            if(guiItem){
                clearSelection(false);
                appendToSelection(guiItem);
                centerItem(guiItem);
            }
        }
    }
}


/**
 * @brief NodeView::goToImplementation
 * If the node is not a definition, check to see if it has a definition.
 * If it does and it has at least 1 implementation, select & center on the first one.
 * @param node
 * @param show
 */
void NodeView::goToImplementation(Node *node)
{
    if (!node) {
        node = getSelectedNode();
    }

    if (node) {
        Node* impl = hasImplementation(node);
        if (impl) {
            // make sure the Workload view aspect is on
            addAspect("Workload");
            GraphMLItem* guiItem = getGraphMLItemFromGraphML(impl);
            if(guiItem){
                clearSelection(false);
                appendToSelection(guiItem);
                centerItem(guiItem);
            }
        }
    }
}

/**
 * @brief NodeView::goToInstance
 * @param node
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
    centerItem(guiItem);
}


/**
 * @brief NodeView::view_deleteSelectedNode
 * This triggers the same actions for when DELETE is pressed.
 */
void NodeView::deleteSelection()
{
    view_SetAttributeModel(0);
    view_Delete(selectedIDs);
}


/**
 * @brief NodeView::centerAspects
 * This sends a signal to the controller to center on the current view aspects.
 */
void NodeView::centerAspects()
{
    sortAspects();
    centerItem(getNodeItemFromNode(controller->getModel()));
    fitToScreen();
}


/**
 * @brief NodeView::sortModel
 * This gets the model from the controller and then sorts it.
 */
void NodeView::sortModel()
{
    if(!controller){
        return;
    }

    Model* model = controller->getModel();

    if(!model){
        qCritical() << "NodeView::view_sortModel() Model from Controller is NULL";
        return;
    }

    NodeItem* modelItem = getNodeItemFromNode(model);
    if(modelItem){
        modelItem->newSort();
    }
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
