#include "nodeview.h"
#include "../Controller/newcontroller.h"
#include "toolbarwidget.h"

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

NodeView::NodeView(bool subView, QWidget *parent):QGraphicsView(parent)
{
    SUB_VIEW = subView;
    RUBBERBAND_MODE = false;
    drawingRubberBand = false;

    CONTROL_DOWN = false;
    SHIFT_DOWN = false;

    setDragMode(ScrollHandDrag);
    setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(500, 500);
    rubberBand->setVisible(false);


    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);


    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setContextMenuPolicy(Qt::CustomContextMenu);

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // set view background-color
    setStyleSheet("QGraphicsView{ background-color: rgba(175,175,175,255); }");

    // create toolbar widget
    toolbar = new ToolbarWidget(this);
    files = new QList<Node*>();
}


void NodeView::setController(NewController *controller)
{
    if(!scene()){
        setScene(new QGraphicsScene(this));
    }

    this->controller = controller;
}

void NodeView::disconnectController()
{
    controller = 0;
}

bool NodeView::isSubView()
{
    return SUB_VIEW;
}

bool NodeView::getControlPressed()
{
    return CONTROL_DOWN;
}

NodeView::~NodeView()
{
    emit view_SetSelectedAttributeModel(0);
}

QRectF NodeView::getVisibleRect( )
{
    QPointF topLeft = mapToScene(0,0);
    QPointF bottomRight = mapToScene(viewport()->width(),viewport()->height());
    return QRectF( topLeft, bottomRight );
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
        NodeItem *nodeItem = dynamic_cast<NodeItem*>(item);
        if(nodeItem){
            nodeItems.append(nodeItem);
        }
    }
    return nodeItems;
}


/**
 * @brief NodeView::showAllViewAspects
 */
void NodeView::showAllViewAspects()
{
    QStringList allAspects;
    allAspects.append("Assembly");
    allAspects.append("Hardware");
    allAspects.append("Definitions");
    allAspects.append("Workload");

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


void NodeView::constructNewView(Node *centeredOn)
{
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

    mainLayout->addWidget(newView);
    newViewWindow->setLayout(mainLayout);
    newViewWindow->show();;

    if(this->controller){
        controller->connectView(newView);

        newView->setViewAspects(currentAspects);

        QList<Node*> toConstruct;
        toConstruct.append(centeredOn->getChildren());
        while(centeredOn->getParentNode()){
            toConstruct.insert(0, centeredOn);
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
 * @param node
 */
void NodeView::sortNode(Node *node)
{
    GraphMLItem* graphMLItem = getGraphMLItemFromGraphML(node);
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(graphMLItem);

    qCritical() << "Sorting: " << node->toString();
    if(nodeItem){
        if(nodeItem->getChildNodeItems().size() == 0){
            qCritical() << "No Children";
            NodeItem* parentItem = nodeItem->getParentNodeItem();
            while(parentItem){
                parentItem->sort();
                parentItem = parentItem->getParentNodeItem();
            }
        }else{
            foreach (NodeItem* child, nodeItem->getChildNodeItems()) {
                sortNode(child->getNode());
            }

            //foreach(Node* child, node->getChildren(0)){
            //    sortNode(node);
           // }
        }
    }
}


bool NodeView::viewportEvent(QEvent * e)
{
    emit updateViewPort(getVisibleRect());
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

    //qDebug() << "Centering " << item->getGraphML()->getDataValue("kind");

    QRectF viewRect = getVisibleRect();
    QRectF itemRect = ((QGraphicsItem*)item)->sceneBoundingRect();
    qreal extraSpace = 1.2;

    QRectF newRec = scene()->itemsBoundingRect();
    qreal multiplier = (viewRect.height() / itemRect.height()) - 1;
    qreal neededXGap = qAbs((viewRect.width() - (itemRect.width()*multiplier)) / 2);
    qreal neededYGap = qAbs((viewRect.height() - (itemRect.height()*multiplier)) / 2);
    qreal leftXGap = qAbs((itemRect.x() - sceneRect().x()) * multiplier);
    qreal rightXGap = qAbs((sceneRect().x() + sceneRect().width() - (itemRect.x() + itemRect.width()))  * multiplier);
    qreal topYGap = qAbs((itemRect.y() - sceneRect().y()) * multiplier);
    qreal bottomYGap = qAbs(((sceneRect().y() + sceneRect().height()) - (itemRect.y() + itemRect.height())) * multiplier);

    // check to make sure that there is enough space around the
    // items boundingRect within the scene before centering it
    // if there isn't, add the needed space to the sceneRect
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

    itemRect.setWidth(itemRect.width()*extraSpace*1.15);
    itemRect.setHeight(itemRect.height()*extraSpace);
    fitInView(itemRect, Qt::KeepAspectRatio);
    centerOn(item);
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

    if(On && !SUB_VIEW){
        RUBBERBAND_MODE = true;
        setDragMode(NoDrag);
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
    view_centerViewAspects();
}


/**
 * @brief NodeView::showContextMenu
 * This is called when there is a mouse right-click event on a node item.
 * @param position
 */
void NodeView::showContextMenu(QPoint position)
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

    // NodeItem* parentNodeItem = getNodeItemFromNode(parentNode);

    NodeItem* nodeItem = new NodeItem(node, parentNodeItem, currentAspects);

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
    if (parentNode && parentNode->getDataValue("kind") == "File")  {
        if (parentNode->childrenCount() == 1) {
            emit updateDockContainer("Parts");
        }
    }

    // update parts dock conatiner everytime there is a newly created
    // node item, in case the current adoptable node list has chnaged
    //emit updateDockContainer("Parts");


    // Stop component definitions and hardware nodes from being
    // drawn on the canvas. Handle differently when user is adding
    // a new definition or connecting hardware nodes.

    //TO FIX: Why have 2 invidual Signals?
    Component* component = dynamic_cast<Component*>(node);
    if (component) {
        emit componentNodeMade("component", nodeItem);

        // store which File the new Component belongs to
        if (nodeItem && parentNode) {
            nodeItem->setFileID(parentNode->getID());
        }

        return;
    }

    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*>(node);
    if (hardwareNode) {
        nodeItem->setHidden(true);
        emit hardwareNodeMade("hardware", nodeItem);
        return;
    }
    // store Files for toolbar menu
    File* file = dynamic_cast<File*>(node);
    if (file) {
        files->append(file);
    }
    /*
    if (nodeItem && node->getDataValue("kind") == "ComponentInstance") {
        qDebug() << "858564651612361654161563161";
        nodeItem->updateParentHeight(nodeItem);
    }
    */
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
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }
}

void NodeView::view_DestructGraphMLGUI(QString ID)
{
    removeGraphMLItemFromHash(ID);
}

void NodeView::view_SelectGraphML(GraphML *graphML, bool setSelected)
{
    if(graphML){
        GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
        if(guiItem){
            guiItem->setSelected(setSelected);

            if(setSelected){
                emit view_SetSelectedAttributeModel(guiItem->getAttributeTable());
                Node* node = dynamic_cast<Node*>(graphML);
                if(node){
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
    qCritical() << "Centering on: " << graphML->toString();
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    if(guiItem){
        centreItem(guiItem);
    }
}

void NodeView::view_LockCenteredGraphML(GraphML *graphML)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    NodeItem* nodeItem = getNodeItemFromGraphMLItem(guiItem);
    if(nodeItem){
        centreItem(guiItem);
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


void NodeView::connectGraphMLItemToController(GraphMLItem *GUIItem, GraphML *graphML)
{

    if(GUIItem){
        connect(GUIItem, SIGNAL(triggerSelected(GraphML*, bool)), controller, SLOT(view_GraphMLSelected(GraphML*, bool)));

        NodeItem* nodeItem = dynamic_cast<NodeItem*>(GUIItem);

        if(nodeItem){
            connect(nodeItem, SIGNAL(clearSelection()), this, SLOT(clearSelection()));
        }



        if(!SUB_VIEW){
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
    NodeItem* node = dynamic_cast<NodeItem*>(item);
    return node;
}

NodeEdge *NodeView::getEdgeItemFromGraphMLItem(GraphMLItem *item)
{
    NodeEdge* edge = dynamic_cast<NodeEdge*>(item);
    return edge;
}


/*
NodeItem *NodeView::getNodeItemFromGraphML(GraphML *item)
{
    Node* node = getNodeFromGraphML(item);
    return getNodeItemFromNode(node);
}

NodeEdge *NodeView::getNodeEdgeFromGraphML(GraphML *item)
{
    Edge* edge = getEdgeFromGraphML(item);
    return getNodeEdgeFromEdge(edge);
}

Node *NodeView::getNodeFromGraphML(GraphML *item)
{
    Node* node = dynamic_cast<Node*>(item);
    return node;
}

Edge *NodeView::getEdgeFromGraphML(GraphML *item)
{
    Edge* edge = dynamic_cast<Edge*>(item);
    return edge;
}
*/

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
            }
            return true;
        }
    }else{
        qCritical() << "Not HERE?!" << ID;
    }
    return false;
}

void NodeView::nodeSelected_signalUpdates(Node *node)
{
    qCritical() << "nodeSelected_signalUpdates()";
    if (node) {

           Node* hasDefn = hasDefinition(node);
           Node* hasImpl = hasImplementation(node);

           // update goto menu actions
           emit setGoToMenuActions("definition", hasDefn);
           emit setGoToMenuActions("implementation", hasImpl);

           // update goto toolbar buttons
           toolbar->showDefinitionButton(hasDefn);
           toolbar->showImplementationButton(hasImpl);

           // update the dock buttons and dock adoptable nodes list
           updateDockButtons(node);
           emit updateDockAdoptableNodesList(node);
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

GraphMLItem *NodeView::getGraphMLItemFromHash(QString ID)
{
    if(guiItems.contains(ID)){
        return guiItems[ID];
    }else{
        qCritical() << "Cannot find GraphMLItem from Lookup Hash. ID: " << ID;
    }
    return 0;
}


/*

NodeEdge *NodeView::getNodeEdgeFromEdge(Edge *edge)
{
    foreach(NodeEdge* nodeEdge, nodeEdges){
        if(nodeEdge->getGraphML() == edge){
            return nodeEdge;
        }
    }
    return 0;
}

NodeItem *NodeView::getNodeItemFromNode(Node *node)
{
    foreach(NodeItem* nodeItem, nodeItems){
        if(nodeItem->getGraphML() == node){
            return nodeItem;
        }
    }
    return 0;
}
*/

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
        QPoint screenFinish = screenOrigin + QPoint(rubberBand->width(),rubberBand->height());
        QPointF sceneOrigin = this->mapToScene(screenOrigin);
        QPointF sceneFinish = this->mapToScene(screenFinish);

        selectedInRubberBand(sceneOrigin, sceneFinish);
        setRubberBandMode(false);
        return;
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

    QPointF scenePos = this->mapToScene(event->pos());

    QGraphicsItem* item = this->scene()->itemAt(scenePos, transform());

    NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);

    if(!nodeItem){
        this->setCursor(Qt::ArrowCursor);
    }

    // enable panning without having to hold CTRL
    //setDragMode(ScrollHandDrag);

    QGraphicsView::mouseMoveEvent(event);
}


void NodeView::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "startMousePress";

        QPointF scenePos = this->mapToScene(event->pos());
        QGraphicsItem* item = this->scene()->itemAt(scenePos, QTransform());
        //rubberBanding = false;

        if(RUBBERBAND_MODE){
            rubberBandOrigin = event->pos();
            //Move rubberband to the position on the screen.
            rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
            drawingRubberBand = true;
            return;
        }

        if(!item){

            if(event->button() == Qt::MiddleButton){
                // sort and center current view aspects
                view_centerViewAspects();
                return;
            }else if(event->button() == Qt::RightButton && CONTROL_DOWN){
                //emit unselect();
                //emit constructNodeItem(scenePos);
            }else if( event->button() == Qt::LeftButton && CONTROL_DOWN){
                qDebug() << "Panning . . .";
                //origin = scenePos.toPoint();
            }else{
                // clear selection and disable dock buttons
                clearSelection();
                return;
            }

        } else {
            // update attribute table size
            if (event->button() == Qt::LeftButton) {
                emit updateDataTable();
            }
        }

        QGraphicsView::mousePressEvent(event);
        //qDebug() << "endMousePress";
}


void NodeView::wheelEvent(QWheelEvent *event)
{
    // enable zooming without having to hold CTRL
    //if(CONTROL_DOWN){

    // Scale the view / do the zoom
    double scaleFactor = 1.05;
    if(event->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zooming out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
    emit updateZoom(transform().m22());

    //}
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
    foreach (QGraphicsItem *itm, scene()->items()) {
        NodeItem *nodeItm = dynamic_cast<NodeItem*>(itm);
        if (nodeItm) {
            nodeItm->resetSize();
        }
    }

    forceSortViewAspects();
    setViewAspects(currentAspects);
}


void NodeView::selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint)
{
    QRectF selectionRectangle(fromScenePoint, toScenePoint);

    //Get the Model and work down.
    QList<QGraphicsItem*> graphicsItems = this->scene()->items(Qt::AscendingOrder);

    NodeItem* modelItem = 0;
    foreach(QGraphicsItem* item, graphicsItems){
        NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);
        if(nodeItem){
            if(nodeItem->getGraphML()->getDataValue("kind") == "Model"){
                modelItem = nodeItem;
                break;
            }
        }
    }


    QList<NodeItem*> nodeItems;
    nodeItems << modelItem;
    while(nodeItems.size() > 0){
        NodeItem* currentNode = nodeItems.takeFirst();

        if(currentNode->intersectsRectangle(selectionRectangle) && currentNode->isVisible() && currentNode->isPainted()){
            controller->view_GraphMLSelected(currentNode->getGraphML(), true);
        }else{
            foreach(QGraphicsItem* childItem, currentNode->childItems()){
                NodeItem* childNode = dynamic_cast<NodeItem*>(childItem);
                if(childNode){
                    nodeItems.push_back(childNode);
                }
            }
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
}


/**
 * @brief NodeView::disableDockButtons
 * This gets called when either the view or the model is pressed.
 * It disables all the dock toggle buttons.
 */
void NodeView::disableDockButtons()
{
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
 * This scales the view to fit the whole scene.
 */
void NodeView::fitToScreen()
{
    QRectF itemsRec = scene()->itemsBoundingRect();
    setSceneRect(itemsRec);
    fitInView(itemsRec, Qt::KeepAspectRatio);
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
 * @brief NodeView::toolbar_deleteNode
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
    if (currentAspects.count() == 1) {
        emit centerNode(currentAspects.at(0));
        return;
    } else if (currentAspects.count() == 2) {
        if (currentAspects.contains("Assembly") && currentAspects.contains("Hardware")){
            emit sortDeployment();
            emit centerNode("Deployment");
            return;
        } else if (currentAspects.contains("Definitions") && currentAspects.contains("Workload")) {
            emit sortModel();
            emit centerNode("Model");
            return;
        }
    }

    showAllViewAspects();
    view_sortModel();
    emit updateViewAspects(currentAspects);
    emit centerNode("Model");
}

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
    emit sortDeployment();
    emit sortModel();
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
