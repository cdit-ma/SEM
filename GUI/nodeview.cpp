#include "nodeview.h"
#include "../Controller/newcontroller.h"
#include "toolbarwidget.h"
#include "docktogglebutton.h"

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
#include <QMenu>
#include <QInputDialog>
#include <QAction>
#include <QVBoxLayout>
#include <QTime>

#define ZOOM_SCALE_INCREMENTOR 1.05
#define ZOOM_SCALE_DECREMENTOR 1.0 / ZOOM_SCALE_INCREMENTOR

NodeView::NodeView(bool subView, QWidget *parent):QGraphicsView(parent)
{
    CENTRALIZED_ON_ITEM = false;
    IS_SUB_VIEW = subView;
    toolbarJustClosed = false;
    parentNodeView = 0;
    dock = 0;
    rubberBand = 0;
    CONTROL_DOWN = false;
    SHIFT_DOWN = false;
    autoCenterOn = true;

    //Construct a Scene for this NodeView.
    setScene(new QGraphicsScene(this));


    //Set QT Options for this QGraphicsView
    setDragMode(ScrollHandDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


    //Set GraphicsView background-color
    setStyleSheet("QGraphicsView{ background-color: rgba(175,175,175,255);}");

    //QBrush brush( QColor(100,100,200,250) );
    //scene()->setBackgroundBrush(brush);

    //Set The rubberband Mode.
    setRubberBandMode(false);

    //Setup Aspects
    allAspects << "Assembly";
    allAspects << "Hardware";
    allAspects << "Definitions";
    allAspects << "Workload";

    //create toolbar widget
    toolbar = new ToolbarWidget(this);

    files = new QList<Node*>();
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

NodeView::~NodeView()
{
    //Clear the current Selected Attribute Model so that the GUI doesn't crash.
    emit view_SetSelectedAttributeModel(0);

    if(this->parentNodeView){
        parentNodeView->removeSubView(this);
        centralizedItemID = "";
        CENTRALIZED_ON_ITEM = false;
    }
}



void NodeView::setDock(DockScrollArea *dock)
{
    this->dock = dock;
    dock->setNodeView(this);
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
    QRectF prevSceneRect = sceneRect();

    qreal multiplier = (viewRect.height() / rectToCenter.height()) - 1;
    qreal neededXGap = qAbs((viewRect.width() - (rectToCenter.width()*multiplier)) / 2);
    qreal neededYGap = qAbs((viewRect.height() - (rectToCenter.height()*multiplier)) / 2);
    qreal leftXGap = qAbs((rectToCenter.x() - sceneRect().x()) * multiplier);
    qreal rightXGap = qAbs((sceneRect().x() + sceneRect().width() - (rectToCenter.x() + rectToCenter.width()))  * multiplier);
    qreal topYGap = qAbs((rectToCenter.y() - sceneRect().y()) * multiplier);
    qreal bottomYGap = qAbs(((sceneRect().y() + sceneRect().height()) - (rectToCenter.y() + rectToCenter.height())) * multiplier);

    // check to make sure that there is enough space around the items boundingRect within
    // the scene before centering it; if there isn't, add the needed space to the sceneRect
    if (leftXGap < neededXGap) {
        newRec.setX(newRec.x()-neededXGap);
        newRec.setWidth(newRec.width()+neededXGap);
        setSceneRect(newRec);
    } else if (rightXGap < neededXGap) {
        newRec.setWidth(newRec.width()+neededXGap);
        setSceneRect(newRec);
    }
    if (topYGap < neededYGap) {
        newRec.setY(newRec.y()-neededYGap);
        newRec.setHeight(newRec.height()+neededYGap);
        setSceneRect(newRec);
    } else if (bottomYGap < neededYGap) {
        newRec.setHeight(newRec.height()+neededYGap);
        setSceneRect(newRec);
    }

    // send a signal to the node items when there has been a change to the sceneRect
    if (newRec.width() != prevSceneRect.width() || newRec.height() != prevSceneRect.height()) {
        //emit sceneRectChanged(newRec);
    }
}


/**
 * @brief NodeView::centerRect
 * This centers the provided rectangle in the view.
 * @param rect
 */
void NodeView::centerRect(QRectF rect)
{
    QPointF rectCenter = rect.center();
    qreal extraSpace = 1.15;

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
void NodeView::showAllViewAspects()
{
    emit updateViewAspects(allAspects);
}


/**
 * @brief NodeView::getSelectedNode
 * @return
 */
Node *NodeView::getSelectedNode()
{
    return controller->getSelectedNode();
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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
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
        if (centeredOn->isImpl() && !currentAspects.contains("Behaviour")) {
            newAspects.append("Behaviour");
        }
        newView->setViewAspects(newAspects);

        QList<Node*> toConstruct;
        toConstruct << centeredOn->getChildren();

        while(centeredOn){
            toConstruct.insert(0,centeredOn);
            centeredOn = centeredOn->getParentNode();
        }

        while(toConstruct.size() > 0){
            newView->view_ConstructGraphMLGUI(toConstruct.takeFirst());
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
    emit triggerAction("View: Sorting entire Model");
    qCritical() << "Sorting Model";
    sortNode(controller->getModel());
    qCritical() <<"Sorting ERROR:";
}


/**
 * @brief NodeView::sortNode
 * This method recursively sorts the provided node.
 * It sorts from the lowest level children back up to the provided node.
 * @todo
 * @param node
 */
void NodeView::sortNode(Node *node, Node* topMostNode)
{
    if (!topMostNode) {
        topMostNode = node;
    }

    GraphMLItem* graphMLItem = getGraphMLItemFromGraphML(node);
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(graphMLItem);

    // check if node has children
    if (nodeItem->getChildNodeItems().count() == 0) {
        // if it doesn't and it also != topMost node, recurse sorting to parent
        if (node != topMostNode) {
            // otherwise, iterate up to the topMost node while sorting node
            while (nodeItem->getParentNodeItem()) {
                nodeItem = nodeItem->getParentNodeItem();
                nodeItem->sort();
                if (nodeItem->getNode() != topMostNode) {
                    break;
                }
            }
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
    emit viewportRectangleChanged(getVisibleRect());
    return QGraphicsView::viewportEvent(e);
}


/**
 * @brief NodeView::centreItem
 * This method scales and translates the scene to center on the item.
 * It extends the scene rect if it's not big enough to centre the item.
 * @param item
 */
void NodeView::centreItem(GraphMLItem *item)
{
    if (!item) {
        qCritical() << "No GUI item to Center";
        return;
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
 * @param aspects
 */
void NodeView::setViewAspects(QStringList aspects)
{
    currentAspects = aspects;
    emit updateViewAspects(aspects);

    if (!IS_SUB_VIEW && autoCenterOn) {
        view_centerViewAspects();
    }

    clearSelection();
}


/**
 * @brief NodeView::showContextMenu
 * This is called when there is a mouse right-click event on a node item.
 * @param position
 */
void NodeView::showToolbar(QPoint position)
{


    QPoint globalPos = mapToGlobal(position);
    menuPosition = mapToScene(position);

    // update toolbar position and connect selected node item
    GraphMLItem* graphmlItem = getGraphMLItemFromGraphML(controller->getSelectedNode());
    if (graphmlItem) {
        NodeItem* nodeItem = getNodeItemFromGraphMLItem(graphmlItem);
        toolbar->setNodeItem(nodeItem);
        toolbar->move(globalPos);
        toolbar->show();
    }
}


/**
 * @brief NodeView::view_DockConstructNode
 * This is called whenever a dock adoptable node item is pressed
 * from the Parts container in the dock.
 * @param kind
 */
void NodeView::view_DockConstructNode(Node* parentNode, QString kind)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(parentNode);
    NodeItem* parentItem = getNodeItemFromGraphMLItem(guiItem);

    // set Initial position so it doesn't collide with any of nodeItem.children();
    if (parentItem) {
        emit constructNode(kind, parentItem->getNextChildPos());
    }
}


void NodeView::view_ConstructNodeGUI(Node *node)
{
    if(!node){
        qCritical() << "Node is Null.";
    }

    Node* parentNode = node->getParentNode();

    NodeItem* parentNodeItem = 0;
    if(parentNode){
        GraphMLItem* parentGUI = getGraphMLItemFromHash(parentNode->getID());
        parentNodeItem = getNodeItemFromGraphMLItem(parentGUI);
    }


    if(IS_SUB_VIEW){
        if(!parentNodeItem && node->getDataValue("kind") != "Model"){
            qCritical() << "NodeView::view_ConstructNodeGUI() SUB_VIEW probably not meant to build this item as we don't have it's parent.";
            return;
        }
    }
    // NodeItem* parentNodeItem = getNodeItemFromNode(parentNode);


    NodeItem* nodeItem = new NodeItem(node, parentNodeItem, currentAspects, IS_SUB_VIEW);

    storeGraphMLItemInHash(nodeItem);
    //nodeItems.append(nodeItem);

    //Connect the Generic Functionality.
    connectGraphMLItemToController(nodeItem, node);


    if(scene() && !scene()->items().contains(nodeItem)){
        //Add to model.
        scene()->addItem(nodeItem);
    }

    // if this item's parent is a File, check if it's the first child
    // if it is, update dock adoptable node list
    //if (parentNode && parentNode->getDataValue("kind") == "File")  {
    //    if (parentNode->childrenCount() == 1) {
    //        emit updateDockContainer("Parts");
    //   }
    // }
    updateDocks();

    // update parts dock conatiner everytime there is a newly created
    // node item, in case the current adoptable node list has chnaged
    //emit updateDockContainer("Parts");



    // Stop component definitions and hardware nodes from being
    // drawn on the canvas. Handle differently when user is adding
    // a new definition or connecting hardware nodes.

    nodeConstructed_signalUpdates(nodeItem);
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
        NodeEdge* nodeEdge = new NodeEdge(edge, srcGUI, dstGUI);

        //Add it to the list of EdgeItems in the Model.
        storeGraphMLItemInHash(nodeEdge);
        //nodeEdges.append(nodeEdge);

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

void NodeView::view_DestructGraphMLGUI(QString ID)
{
    removeGraphMLItemFromHash(ID);
    if(IS_SUB_VIEW){
        if(CENTRALIZED_ON_ITEM && centralizedItemID == ID){
            //CALL DELETE ON DIALOG
            this->parent()->deleteLater();
        }
    }
}

void NodeView::view_SelectGraphML(GraphML *graphML, bool setSelected)
{
    if(graphML){
        GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
        if(guiItem){
            guiItem->setSelected(setSelected);

            if(setSelected){
                emit view_SetSelectedAttributeModel(guiItem->getAttributeTable());

                NodeItem* nodeItem = getNodeItemFromGraphMLItem(guiItem);
                if(nodeItem){
                    toolbar->setCurrentNodeItem(nodeItem);

                    //dock->setCurrentNodeItem(nodeItem);
                    updateDocks();
                }


                Node* node = dynamic_cast<Node*>(graphML);
                if(node){
                    //toolbar
                    nodeSelected_signalUpdates(node);
                }



                return;
            }
        }
    }

    emit view_SetSelectedAttributeModel(0);
}

void NodeView::view_SortNode(Node *node)
{
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(node->getID()));
    if(nodeItem){
        nodeItem->sort();
    }

}

void NodeView::view_CenterGraphML(GraphML *graphML)
{
    //qCritical() << "Centering on: " << graphML->toString();
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    if(guiItem){
        centreItem(guiItem);
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

        centreItem(guiItem);
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
 * @brief NodeView::view_ConstructNodeAction
 * @param nodeKind
 */
void NodeView::view_ConstructNode(QString nodeKind)
{
    emit triggerAction("Toolbar: Constructing Node");


    GraphMLItem* selected = getGraphMLItemFromGraphML(getSelectedNode());
    if(selected){
        NodeItem* nodeItem = getNodeItemFromGraphMLItem(selected);
        if(nodeItem){
            emit constructNode(nodeKind, nodeItem->mapFromScene(menuPosition));
        }else{
            //No Node Selected!
        }
    }
}


/**
 * @brief NodeView::view_ConstructEdgeAction
 * @param src
 * @param dst
 */
void NodeView::view_ConstructEdge(Node *src, Node *dst)
{
    emit triggerAction("Toolbar: Constructing Edge");
    emit constructEdge(src, dst);
    edgeConstructed_signalUpdates(src);
}


/**
 * @brief NodeView::view_addComponentInstance
 * @param assm - ComponentAssembly (selected node)
 * @param defn - Component from selected dock/toolbar item
 * @param sender - 0 = DockScrollArea, 1 = ToolbarWidget
 */
void NodeView::view_ConstructComponentInstance(Node* assm, Node* defn, int sender)
{
    emit triggerAction("Toolbar: Constructing ComponentInstance");

    ComponentAssembly* assembly = dynamic_cast<ComponentAssembly*>(assm);
    Component* definition = dynamic_cast<Component*>(defn);

    if (assembly && definition) {
        GraphMLItem *graphMLItem = getGraphMLItemFromGraphML(assembly);
        if (sender == 0) {
            NodeItem *nodeItem = getNodeItemFromGraphMLItem(graphMLItem);
            emit constructComponentInstance(assembly, definition, nodeItem->getNextChildPos());
        } else if (sender == 1) {
            emit constructComponentInstance(assembly, definition, graphMLItem->mapFromScene(menuPosition));
        }

    }
}


/**
 * @brief NodeView::view_connectComponentDefinition
 * @param itm
 *
void NodeView::view_connectComponentInstance(Node *inst, Node *defn)
{
    emit triggerAction("Toolbar: Connecting ComponentInstance to Component");
    if (inst && defn) {
        emit constructEdge(inst, defn);
    }
}*/


/**
 * @brief NodeView::updateToolbarMenuList
 * @param action
 * @param node
 */
void NodeView::updateToolbarMenuList(QString action, Node *node)
{
    if (action == "add") {
        emit getAdoptableNodesList(node);
    } else if (action == "connect") {
        emit getLegalNodesList(node);
    } else if (action == "addInstance") {
        emit getComponentDefinitions(node);
    } else if (action == "files") {
        emit updateMenuList("files", 0, files);
    }
}


/**
 * @brief NodeView::updateToolbarAdoptableNodeList
 * This is called by NewMedeaWindow whenever the toolbar adoptable nodes list
 * needs to be updated. If selectedNode == prevSelectedNode, nothing happens.
 * @param action
 * @param nodeList
 */
void NodeView::updateToolbarAdoptableNodesList(QStringList nodeKinds)
{
    QStringList* kinds = new QStringList(nodeKinds);
    emit updateMenuList("add", kinds, 0);
}


/**
 * @brief NodeView::updateToolbarLegalNodesList
 * @param nodeList
 */
void NodeView::updateToolbarLegalNodesList(QList<Node*>* nodeList)
{
    emit updateMenuList("connect", 0, nodeList);
}


/**
 * @brief NodeView::updateToolbarDefinitionsList
 * @param nodeList
 */
void NodeView::updateToolbarDefinitionsList(QList<Node*>* nodeList)
{
    emit updateMenuList("addInstance", 0, nodeList);
}

void NodeView::componentInstanceConstructed(Node *node)
{
    GraphMLItem* graphMLItem = getGraphMLItemFromGraphML(node);
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(graphMLItem);
    if (nodeItem) {
        nodeItem->updateParentHeight(nodeItem);
    }
}

NewController *NodeView::getController()
{
    return this->controller;
}

QList<Node *> *NodeView::getFiles()
{
    Model* model = this->controller->getModel();

    if(model){
        return new QList<Node*>(model->getChildrenOfKind("File"));
    }

    return 0;
}

void NodeView::updateDocks()
{

    if(dock){
        if(dock->getParentButton()->getKind() == "P"){

            dock->updatePartsDock();
        }
    }
}




void NodeView::connectGraphMLItemToController(GraphMLItem *GUIItem, GraphML *graphML)
{

    if(GUIItem){
        connect(GUIItem, SIGNAL(triggerSelected(GraphML*, bool)), controller, SLOT(view_GraphMLSelected(GraphML*, bool)));

        NodeItem* nodeItem = 0;
        if(GUIItem->isNodeItem()){
            nodeItem = (NodeItem*) GUIItem;
        }

        if(nodeItem){
            connect(nodeItem, SIGNAL(clearSelection()), this, SLOT(clearSelection()));
        }



        if(!IS_SUB_VIEW){
            connect(GUIItem, SIGNAL(triggerCentered(GraphML*)), this, SLOT(view_CenterGraphML(GraphML*)));
            connect(GUIItem, SIGNAL(triggerAction(QString)),  controller, SLOT(view_TriggerAction(QString)));

            connect(GUIItem, SIGNAL(constructGraphMLData(GraphML*,QString)), controller, SLOT(view_ConstructGraphMLData(GraphML*,QString)));
            connect(GUIItem, SIGNAL(destructGraphMLData(GraphML*,QString)), controller, SLOT(view_DestructGraphMLData(GraphML*,QString)));
            connect(GUIItem, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), controller, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));


            //Connect the Node Specific Functionality


            if(nodeItem){
                connect(nodeItem, SIGNAL(moveSelection(QPointF)), controller, SLOT(view_MoveSelectedNodes(QPointF)));
                connect(this, SIGNAL(updateViewAspects(QStringList)), nodeItem, SLOT(updateViewAspects(QStringList)));
                connect(nodeItem, SIGNAL(centerViewAspects()), this, SLOT(view_centerViewAspects()));
                connect(nodeItem, SIGNAL(sortModel()), this, SLOT(view_sortModel()));
                connect(nodeItem, SIGNAL(updateDockContainer(QString)), this, SLOT(view_updateDockContainer(QString)));
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

NodeEdge *NodeView::getEdgeItemFromGraphMLItem(GraphMLItem *item)
{

    if(item && item->isNodeEdge()){
        return (NodeEdge*) item;
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

        if(item){
            disconnect(item, SIGNAL(updateGraphMLData(GraphML*, QString, QString)), controller, SLOT(view_UpdateGraphMLData(GraphML*, QString, QString)));
            if(scene()->items().contains(item)){
                scene()->removeItem(item);
                delete item;
                updateDocks();
            }
            return true;
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
        if(hasDefn){
            emit setGoToMenuActions("definition", hasDefn);
        }
        if(hasImpl){
            emit setGoToMenuActions("implementation", hasImpl);
        }


        // update goto toolbar buttons
        toolbar->showDefinitionButton(hasDefn);
        toolbar->showImplementationButton(hasImpl);

        // update the dock buttons and dock adoptable nodes list
        updateDockButtons(node);
        emit updateDockAdoptableNodesList(node);
    }

}

/**
 * @brief NodeView::nodeConstructed_signalUpdates
 * This is called whenever a node is constructed.
 * It sends signals to update whatever needs updating.
 * @param node
 */
void NodeView::nodeConstructed_signalUpdates(NodeItem *nodeItem)
{
    // update parts dock conatiner everytime there is a newly created
    // node item, in case the current adoptable node list has chnaged
    // emit updateDockContainer("Parts");

    // store Files for toolbar menu
    File* file = dynamic_cast<File*>(nodeItem->getNode());
    if (file) {
        files->append(file);
        return;
    }

    QString nodeKind = nodeItem->getNode()->getDataValue("kind");
    if (nodeKind == "Component") {
        emit componentNodeMade("component", nodeItem);
    } else if (nodeKind == "HardwareNode") {
        nodeItem->setHidden(true);
        emit hardwareNodeMade("hardware", nodeItem);
    }
}


void NodeView::edgeConstructed_signalUpdates(Node *src)
{
    Node* hasDefn = hasDefinition(src);
    Node* hasImpl = hasImplementation(src);

    // update specific tool buttons when a new edge is constructed
    toolbar->showDefinitionButton(hasDefn);
    toolbar->showImplementationButton(hasImpl);
    toolbar->updateMenuList("connect", src);

    // update node goto menu actions
    emit setGoToMenuActions("definition", hasDefn);
    emit setGoToMenuActions("implementation", hasImpl);
}

Node *NodeView::hasDefinition(Node *node)
{
    Node* original = node;

    while(node->getDefinition()){
        node = node->getDefinition();
    }
    if(node != original){
        return node;
    }
    return 0;
}

Node *NodeView::hasImplementation(Node *node)
{
    Node* original = node;
    node = hasDefinition(node);
    if(!node){
        node = original;
    }
    //Top Most Node.
    if(node->getImplementations().size() > 0){
        Node* impl =node->getImplementations().at(0);
        if(impl != original){
            return impl;
        }
    }

    return 0;
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
        if(!IS_SUB_VIEW){
            qCritical() << "Cannot find GraphMLItem from Lookup Hash. ID: " << ID;
        }
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
            view_centerViewAspects();
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


void NodeView::wheelEvent(QWheelEvent *event)
{
    // Scale the view / do the zoom
    if(event->delta() > 0) {
        // Zoom in
        scale(ZOOM_SCALE_INCREMENTOR, ZOOM_SCALE_INCREMENTOR);
    } else {
        // Zooming out
        scale(ZOOM_SCALE_DECREMENTOR, ZOOM_SCALE_DECREMENTOR);
    }
}

void NodeView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete){
        emit deletePressed(true);
    }

    if(event->key() == Qt::Key_Escape){
        emit escapePressed(true);
        emit updateDockButtons("N");
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_A){
        emit selectAll();
    }

    if(event->key() == Qt::Key_Shift){
        this->SHIFT_DOWN = true;
        emit shiftPressed(true);
    }
    if(event->key() == Qt::Key_Control){
        //Use ScrollHand Drag Mode to enable Panning
        this->CONTROL_DOWN = true;
        emit controlPressed(true);
    }

    if(CONTROL_DOWN && SHIFT_DOWN){
        setRubberBandMode(true);
    }else{
        if(RUBBERBAND_MODE){
            setRubberBandMode(false);
        }
    }


}

void NodeView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        this->CONTROL_DOWN = false;
        emit controlPressed(false);
    }
    if(event->key() == Qt::Key_Shift){
        this->SHIFT_DOWN = false;
        emit shiftPressed(false);
    }
}

void NodeView::view_ClearHistory()
{
    emit view_ClearHistoryStates();
}

void NodeView::toolbarClosed()
{
    toolbarJustClosed = true;
}



/**
 * @brief NodeView::forceSortViewAspects
 */
void NodeView::forceSortViewAspects()
{
    showAllViewAspects();
    view_sortModel();
}



/**
 * @brief NodeView::resetModel
 * This method is called after the model is cleared.
 * It resets the size of the model, sorts and centers it.
 */
void NodeView::resetModel()
{
    triggerAction("Resetting Model");
    foreach(NodeItem* nodeItem, getNodeItemsList()){
        if (nodeItem) {
            nodeItem->resetSize();
        }
    }

    forceSortViewAspects();
    setViewAspects(currentAspects);
}


/**
 * @brief NodeView::autoCenterViewAspects
 * @param center
 */
void NodeView::setAutoCenterViewAspects(bool center)
{
    autoCenterOn = center;
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

    while(nodeItems.size() > 0){
        NodeItem* currentNode = nodeItems.takeFirst();
        Node* node = currentNode->getNode();

        if(currentNode->intersectsRectangle(selectionRectangle) && currentNode->isVisible() && currentNode->isPainted()){
            controller->view_GraphMLSelected(node, true);
        }else{
            nodeItems << currentNode->getChildNodeItems();
        }
    }
}


/**
 * @brief NodeView::centreNode
 * This gets called every time the user middle clicks on the view.
 * @param node
 */
void NodeView::centreNode(Node *node)
{
    if (node) {
        centreItem(getGraphMLItemFromGraphML(node));
    }
}


/**
 * @brief NodeView::clearSelection
 * This gets called when either the view or the model is pressed.
 * It clears the selection.
 */
void NodeView::clearSelection()
{
    emit unselect();
    emit updateDockButtons("N");
}


void NodeView::view_ConstructGraphMLGUI(GraphML *item){

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


void NodeView::printErrorText(GraphML *graphml, QString text)
{
    //TODO
    Q_UNUSED(graphml);
    Q_UNUSED(text);
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
            if ((pf.x()+nodeItem->boundingRect().width()) > rightMostX) {
                rightMostX = pf.x() + nodeItem->boundingRect().width();
            }
            if (pf.y() < topMostY) {
                topMostY = pf.y();
            }
            if ((pf.y()+nodeItem->boundingRect().height()) > bottomMostY) {
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
void NodeView::goToDefinition(Node *node, bool show)
{
    if (node) {
        Node* defn = hasDefinition(node);
        if (defn) {
            // make sure the Definitions view aspect is on
            if (!currentAspects.contains("Definitions")) {
                emit turnOnViewAspect("Definitions");
            }
            controller->view_GraphMLSelected(defn, true);
            centreItem(getGraphMLItemFromGraphML(defn));
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
void NodeView::goToImplementation(Node *node, bool show)
{
    if (node) {
        Node* impl = hasImplementation(node);
        if (impl) {
            // make sure the Definitions view aspect is on
            if (!currentAspects.contains("Workload")) {
                emit turnOnViewAspect("Workload");
            }
            controller->view_GraphMLSelected(impl, true);
            centreItem(getGraphMLItemFromGraphML(impl));
        }
    }
}


/**
 * @brief NodeView::view_deleteSelectedNode
 * This triggers the same actions for when DELETE is pressed.
 */
void NodeView::view_deleteSelectedNode()
{
    emit deletePressed(true);
}


/**
 * @brief NodeView::view_centerViewAspects
 * This sends a signal to the controller to center on the current view aspects.
 */
void NodeView::view_centerViewAspects()
{
    forceSortViewAspects();
    emit updateViewAspects(currentAspects);
    emit centerNode("Model");
    fitToScreen();
}


/**
 * @brief NodeView::setGoToToolbarButtons
 * @param action
 * @param node
 */
void NodeView::setGoToToolbarButtons(QString action, Node *node)
{
    if (action == "definition") {
        toolbar->showDefinitionButton(hasDefinition(node));
    } else if (action == "implementation") {
        toolbar->showImplementationButton(hasImplementation(node));
    }
}


/**
 * @brief NodeView::view_sortModel
 */
void NodeView::view_sortModel()
{


    Model* model = controller->getModel();

    if(!model){
        qCritical() << "NodeView::view_sortModel() Model from Controller is NULL";
        return;
    }
    QList<Node*> deploymentDefinitions = model->getChildrenOfKind("DeploymentDefinitions", 0);

    if(deploymentDefinitions.size() != 1){
        qCritical() << "NodeView::view_sortModel() Model doens't contain a DeploymentDefinition!";
        return;
    }


    NodeItem* deploymentDefinitionItem = getNodeItemFromNode(deploymentDefinitions.at(0));
    NodeItem* modelItem = getNodeItemFromNode(model);

    if(deploymentDefinitionItem){
        deploymentDefinitionItem->sort();
    }

    if(modelItem){
        modelItem->sort();
    }
}


/**
 * @brief NodeView::updateDockButtons
 * This method enable/disable dock buttons depending on the currently selected node.
 */
void NodeView::updateDockButtons(Node* node)
{
    if (node) {

        QString nodeKind = node->getDataValue("kind");

        if (nodeKind == "InterfaceDefinitions"||
                nodeKind == "BehaviourDefinitions" ||
                nodeKind == "AssemblyDefinitions" ||
                nodeKind == "File" ||
                nodeKind == "Component") {

            emit updateDockButtons("P");

        } else if (nodeKind == "HardwareDefinitions" ||
                   nodeKind == "HardwareCluster" ||
                   nodeKind == "ManagementComponent") {

            emit updateDockButtons("H");

        } else if (nodeKind == "Model" ||
                   nodeKind == "DeploymentDefinitions") {

            emit updateDockButtons("N");

        } else if (nodeKind == "ComponentAssembly") {

            emit updateDockButtons("A");

        } else if (nodeKind == "ComponentImpl") {

            emit updateDockButtons("PD");

        }  else if (nodeKind == "ComponentInstance") {

            if (node->getDefinition()) {
                emit updateDockButtons("H");
            } else {
                emit updateDockButtons("D");
            }

        } else {
            emit updateDockButtons("A");
        }
    }
}


/**
 * @brief NodeView::view_updateDockContainer
 * @param dockContainer
 */
void NodeView::view_updateDockContainer(QString dockContainer)
{
    emit updateDockContainer(dockContainer);
}
