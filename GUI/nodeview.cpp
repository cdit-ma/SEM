#include "nodeview.h"

//Qt includes
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QTouchEvent>
#include <QDebug>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QInputDialog>
#include <QAction>
#include "../Controller/newcontroller.h"

NodeView::NodeView(QWidget *parent):QGraphicsView(parent)
{
    totalScaleFactor = 1;
    NodeType = "";
    rubberBanding = false;
    once = true;

    firstSort = true;

    CONTROL_DOWN = false;
    SHIFT_DOWN = false;
    //setDragMode(RubberBandDrag);
    setDragMode(ScrollHandDrag);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(500, 500);

    //setAlignment(Qt::AlignHCenter);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene
    QGraphicsScene* Scene = new QGraphicsScene(this);
    setScene(Scene);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    //Set-up the view
    // changed the sceneRect values
    // it used to be 19200 x 10800
    // we may not need this here
    //setSceneRect(0, 0, 29200, (10800/2*5));
    //translate(9600,5040);


    // set view background-color
    setStyleSheet("QGraphicsView{ background-color: rgba(150,150,150,255); }");
}

void NodeView::setController(NewController *controller)
{
    this->controller = controller;
}

bool NodeView::getControlPressed()
{
    return this->CONTROL_DOWN;
}

NodeView::~NodeView()
{
    emit view_SetSelectedAttributeModel(0);
}

void NodeView::updateNodeTypeName(QString name)
{
    this->NodeType = name;
    //qCritical() << "NodeType: " << name;

    emit this->updateNodeType(NodeType);
}



void NodeView::removeNodeItem(NodeItem *item)
{
    if(scene()->items().contains(item)){
        scene()->removeItem(item);
    }
}



QRectF NodeView::getVisibleRect( )
{
    QPointF topLeft = mapToScene(0,0);
    QPointF bottomRight = mapToScene(viewport()->width(),viewport()->height());
    return QRectF( topLeft, bottomRight );
}


void NodeView::centreItem(GraphMLItem *item)
{
    if(!item){
        qCritical() << "No GUI item to Center";
        return;
    }

    //Get the current Viewport Rectangle
    QRectF viewRect = getVisibleRect();

    //Get the Items Rectangle
    QRectF itemRect = ((QGraphicsItem*)item)->sceneBoundingRect();

    //Extra Space denotes 20% extra space on the height.
    //Calculate the scalre required to fit the item + 20% in the Viewport Rectangle.
    qreal extraSpace;

    if (item->getGraphML()->getDataValue("kind") == "Model") {
        extraSpace = 1;
    } else {
        extraSpace = 1.25;
    }

    //qreal scaleRatio = viewRect.height()/ (itemRect.height() * extraSpace);
    //scale(scaleRatio, scaleRatio);
    //centerOn(item);


    QRectF newRec = sceneRect();
    qreal multiplier = viewRect.height() / itemRect.height();
    qreal leftXGap = itemRect.x() - sceneRect().x();
    qreal rightXGap = sceneRect().x() + sceneRect().width() - (itemRect.x() + itemRect.width());
    qreal topYGap = itemRect.y() - sceneRect().y();
    qreal bottomYGap = (sceneRect().y() + sceneRect().height()) - (itemRect.y() + itemRect.height());
    qreal neededXGap = qAbs((viewRect.width() - (itemRect.width()*multiplier)) / 2);
    qreal neededYGap = qAbs(viewRect.height()/2);

    //setSceneRect(scene()->itemsBoundingRect());
    itemRect.setHeight(itemRect.height()*extraSpace);
    fitInView(itemRect, Qt::KeepAspectRatio);

    // check to make sure that there is enough space around the
    // items boundingRect within the scene before centering it
    // if there isn't, add the needed space to the sceneRect
    if (item->getGraphML()->getDataValue("kind") != "Model") {
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
    }

    centerOn(item);


    /**
    //Get the actual Scale Ratio!
    //scaleRatio = transform().m22();

    //Get the Center of the ViewPort Rectangle
    QPointF viewCenter = viewRect.center();

    //Get the Center of the Item Rectangle.
    QPointF itemCenter = itemRect.center();

    //Get the updated visible Rectangle.
    viewCenter = getVisibleRect().center();

    //Calculate the distance including scale Ratio between the center of the view and item.
    float deltaX = (itemCenter.x() - viewCenter.x()) * scaleRatio;
    float deltaY = (itemCenter.y() - viewCenter.y()) * scaleRatio;

    //Move the Scroll bars the appropriate distance to square up the items.
    int xBarValue = horizontalScrollBar()->value() + deltaX;
    int yBarValue = verticalScrollBar()->value() + deltaY;

    //horizontalScrollBar()->setValue(xBarValue);
    //verticalScrollBar()->setValue(yBarValue);
    */
}

void NodeView::clearView()
{
    scene()->clear();
    viewport()->update();
}

void NodeView::depthChanged(int depth)
{
    foreach(QGraphicsItem* item, scene()->items()){
        NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);
        if(nodeItem != 0){
            nodeItem->toggleDetailDepth(depth);
        }
    }

}

void NodeView::setRubberBandMode(bool On)
{
    if(On){
        setDragMode(RubberBandDrag);
    }else{
        setDragMode(ScrollHandDrag);
    }


}


void NodeView::setViewAspects(QStringList aspects)
{
    //qCritical() << "Setting: " << aspects;
    currentAspects = aspects;
    emit updateViewAspects(aspects);
}

void NodeView::showContextMenu(QPoint position)
{
    //Got Right Click. Build Menu.
    QPoint globalPos = mapToGlobal(position);
    QPointF scenePos = mapToScene(position);
    qCritical() << "Make Menu";


    menuPosition = scenePos;

    QMenu* rightClickMenu = new QMenu(this);

    QAction* deleteAction = new QAction(this);
    deleteAction->setText("Delete Selection");
    connect(deleteAction, SIGNAL(triggered()), controller, SLOT(view_DeletePressed()));
    rightClickMenu->addAction(deleteAction);

    if(controller->getAdoptableNodeKinds().size() > 0){
        QAction* addChildNode = new QAction(this);
        addChildNode->setText("Create Child Node");
        connect(addChildNode, SIGNAL(triggered()), this, SLOT(view_ConstructNodeAction()));
        rightClickMenu->addAction(addChildNode);
    }

    rightClickMenu->exec(globalPos);
}


/**
 * @brief NodeView::view_DockConstructNode
 * This is called whenever a dock adoptable node item is pressed
 * from the Parts container in the dock.
 * @param kind
 */
void NodeView::view_DockConstructNode(QString kind)
{
    emit constructNodeItem(kind, QPointF(0,0));
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

    NodeItem* nodeItem = new NodeItem(node, parentNodeItem);
    //qDebug() << "Adding " << nodeItem->getGraphML()->getDataValue("label");

    storeGraphMLItemInHash(nodeItem);
    //nodeItems.append(nodeItem);

    //Connect the Generic Functionality.
    connectGraphMLItemToController(nodeItem, node);

    //Connect the Node Specific Functionality
    connect(nodeItem, SIGNAL(moveSelection(QPointF)), controller, SLOT(view_MoveSelectedNodes(QPointF)));
    connect(controller, SIGNAL(view_SetRubberbandSelectionMode(bool)), nodeItem, SLOT(setRubberbandMode(bool)));
    connect(this, SIGNAL(updateNodeType(QString)), nodeItem, SLOT(updateChildNodeType(QString)));
    connect(this, SIGNAL(updateViewAspects(QStringList)), nodeItem, SLOT(updateViewAspects(QStringList)));

    //connect(nodeItem, SIGNAL(updateSceneRect(NodeItem*)), this, SLOT(resetSceneRect(NodeItem*)));
    connect(nodeItem, SIGNAL(clearSelection()), this, SLOT(clearSelection()));


    if(!scene()->items().contains(nodeItem)){
        //Add to model.
        scene()->addItem(nodeItem);
    }


    /************************************************************************************/

    //if this item's parent is a File, check if it's the first child
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

    Component* component = dynamic_cast<Component*>(node);
    if (component) {
        emit componentNodeMade("component", nodeItem);
        return;
    }

    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*>(node);
    if (hardwareNode) {
        nodeItem->setHidden(true);
        emit hardwareNodeMade("hardware", nodeItem);
        return;
    }

}


void NodeView::view_ConstructEdgeGUI(Edge *edge)
{
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    NodeItem* srcGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(src->getID()));
    NodeItem* dstGUI = getNodeItemFromGraphMLItem(getGraphMLItemFromHash(dst->getID()));

    //NodeItem* parentNodeItem = getNodeItemFromGraphMLItem(parentGUI);

    //
    // NodeItem* srcGUI = getNodeItemFromNode(src);
    // NodeItem* dstGUI = getNodeItemFromNode(dst);

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

        if(nodeEdge){
            nodeEdge->addToScene(scene());
        }

    }else{
        qCritical() << "GraphMLController::model_MakeEdge << Cannot add Edge as Source or Destination is null!";
    }

}

void NodeView::view_DestructGraphMLGUI(QString ID)
{
    removeGraphMLItemFromHash(ID);
}

/*
void NodeView::view_DestructGraphMLGUI(GraphML *graphML)
{
    qCritical() << "DELETING?!";
    if(!graphML){
        qCritical() << "NULL";
    }


    NodeItem* nodeItem = getNodeItemFromGraphML(graphML);
    NodeEdge* nodeEdge = getNodeEdgeFromGraphML(graphML);

    qCritical() << "Got Node Item";
    if(nodeItem){
        int nodeItemPosition = nodeItems.indexOf(nodeItem);
        if(nodeItemPosition != -1){
            nodeItems.removeAt(nodeItemPosition);
        }
    }
    else if(nodeEdge){
        int nodeEdgePosition = nodeEdges.indexOf(nodeEdge);

        if(nodeEdgePosition != -1){
            nodeEdges.removeAt(nodeEdgePosition);
        }
    }else{
        qCritical() << "NOT A VALID GRAPHML";
    }


}
*/


void NodeView::view_SelectGraphML(GraphML *graphML, bool setSelected)
{
    if(graphML){

        GraphMLItem* GUIItem = getGraphMLItemFromHash(graphML->getID());

        if(GUIItem){
            GUIItem->setSelected(setSelected);
            if(setSelected){
                emit view_SetSelectedAttributeModel(GUIItem->getAttributeTable());

                // update the dock node items when a node is selected
                // and enable/disable dock buttons accordingly
                Node* node = dynamic_cast<Node*>(graphML);
                emit updateAdoptableNodeList(node);
                updateDockButtons(node);

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
        nodeItem->sortChildren();
        // set/update the sceneRect when the model is sorted/resized
        if (node->getDataValue("kind") == "Model") {
            //resetSceneRect(nodeItem);
        }
    }

}

void NodeView::view_CenterGraphML(GraphML *graphML)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    centreItem(guiItem);
}

void NodeView::view_SetOpacity(GraphML *graphML, qreal opacity)
{
    GraphMLItem* guiItem = getGraphMLItemFromGraphML(graphML);
    if(guiItem){
        guiItem->setOpacity(opacity);
    }
}

void NodeView::view_ConstructNodeAction()
{

    QStringList nodeKinds = controller->getAdoptableNodeKinds();

    if(nodeKinds.size() > 0){
        bool okay;
        QInputDialog* dialog = new QInputDialog();

        QString nodeKind = dialog->getItem(0, "Selected Node Type","Please Select Child Node Type: ", nodeKinds, 0, false, &okay);

        if(!okay){
            return;
        }

        QGraphicsItem* item = this->scene()->itemAt(menuPosition,this->transform());
        if(item){
            emit constructNodeItem(nodeKind, item->mapFromScene(menuPosition));
        }else{
            emit constructNodeItem(nodeKind, menuPosition);
        }
    }else{
        emit controller->view_DialogMessage(MESSAGE_TYPE::WARNING, "No Adoptable Types.");
    }




}

void NodeView::connectGraphMLItemToController(GraphMLItem *GUIItem, GraphML *graphML)
{
    connect(GUIItem, SIGNAL(triggerAction(QString)),  controller, SLOT(view_TriggerAction(QString)));
    connect(GUIItem, SIGNAL(triggerCentered(GraphML*)), this, SLOT(view_CenterGraphML(GraphML*)));
    connect(GUIItem, SIGNAL(triggerSelected(GraphML*, bool)), controller, SLOT(view_GraphMLSelected(GraphML*, bool)));
    connect(GUIItem, SIGNAL(constructGraphMLData(GraphML*,QString)), controller, SLOT(view_ConstructGraphMLData(GraphML*,QString)));
    connect(GUIItem, SIGNAL(destructGraphMLData(GraphML*,QString)), controller, SLOT(view_DestructGraphMLData(GraphML*,QString)));
    connect(GUIItem, SIGNAL(updateGraphMLData(GraphML*,QString,QString)), controller, SLOT(view_UpdateGraphMLData(GraphML*,QString,QString)));
    //connect(graphML, SIGNAL(destroyed()), GUIItem, SLOT(destructGraphML()));
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
        //qCritical() << "Hash Removed ID: " << ID;
        GraphMLItem* item = guiItems[ID];

        guiItems.remove(ID);

        if(item){
            delete item;
            return true;
        }
    }else{
        qCritical() << "Not HERE?!" << ID;
    }
    return false;
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


    QGraphicsView::mouseReleaseEvent(event);
}

void NodeView::mouseMoveEvent(QMouseEvent *event)
{

    QPointF scenePos = this->mapToScene(event->pos());

    QGraphicsItem* item = this->scene()->itemAt(scenePos,this->transform());

    NodeItem* node = dynamic_cast<NodeItem*>(item);

    if(node){
        //this->setCursor(Qt::SizeAllCursor);
    }else{
        this->setCursor(Qt::ArrowCursor);
    }

    QGraphicsView::mouseMoveEvent(event);

    // this removes the non-disappearing blue
    // lines after moving node items
    //update();
}

void NodeView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = this->mapToScene(event->pos());
    QGraphicsItem* item = this->scene()->itemAt(scenePos, QTransform());
    rubberBanding = false;

    if(!item){
        if(event->button() == Qt::MiddleButton){
            if(CONTROL_DOWN){
                emit sortModel();
                return;
            }else{
                /**
                resetTransform();
                int xValue = (horizontalScrollBar()->minimum() + horizontalScrollBar()->maximum())/2;
                int yValue = (verticalScrollBar()->minimum() + verticalScrollBar()->maximum())/2;
                horizontalScrollBar()->setValue(xValue);
                verticalScrollBar()->setValue(yValue);
                */

                // center the model
                emit centerModel();
                return;
            }
        }else if(event->button() == Qt::RightButton && CONTROL_DOWN){
            //emit unselect();
            //emit constructNodeItem(scenePos);
        }else if( event->button() == Qt::LeftButton && CONTROL_DOWN){
            origin = scenePos.toPoint();
        }else{
            // clear selection and disable dock buttons
            clearSelection();
            return;
        }

    } else {
        // sorting node item's children, need to update view
        if (CONTROL_DOWN) {
            update();
        }
        // update attribute table size
        if (event->button() == Qt::LeftButton) {
            emit updateDataTable();
        }
    }

    QGraphicsView::mousePressEvent(event);

}

void NodeView::wheelEvent(QWheelEvent *event)
{
    if(CONTROL_DOWN){
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
        //emit updateViewMargin();
    }else{
        //QGraphicsView::wheelEvent(event);
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

    if(this->CONTROL_DOWN && event->key() == Qt::Key_C){
        // emit copy();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_X){
        //emit cut();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_V){
        //emit paste();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_Z){
        //emit undo();
    }

    if(this->CONTROL_DOWN && event->key() == Qt::Key_Y){
        //emit redo();
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
    /*
    if(event->key() == Qt::Key_Escape){
        emit escapePressed(false);
    }
    */


}


/**
 * @brief NodeView::resetModel
 * This method is called after the model is cleared.
 * It resets the size of the model, sorts and centers it.
 */
void NodeView::resetModel()
{
    qDebug() << "Resetting model...";
    foreach (QGraphicsItem *itm, scene()->items()) {
        NodeItem *nodeItm = dynamic_cast<NodeItem*>(itm);
        if (nodeItm) {
            qDebug() << nodeItm->getGraphML()->getDataValue("label");
            nodeItm->resetSize();
        }
    }
    sortModel();
    centerModel();
    update();
}


/**
 * @brief NodeView::centreModel
 * This gets called every time the user middle clicks on the view.
 * @param node
 */
void NodeView::centreModel(Node *node)
{
    if (node) {
        centreItem(getGraphMLItemFromGraphML(node));
    }
}


/**
 * @brief NodeView::clearSelection
 * This gets called when either the view or the model is pressed.
 * It clears the selection and disables all dock node buttons.
 */
void NodeView::clearSelection()
{
    emit unselect();
    emit updateDockButtons("N");
}


void NodeView::view_Refresh()
{
    scene()->update();
}

/*
bool NodeView::guiCreated(GraphML *item)
{
    return getNodeItemFromGraphML(item) || getNodeEdgeFromGraphML(item);
}
*/

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
    // make new scene rect slightly bigger than the itemsBoundingRect
    QRectF itemsRec = scene()->itemsBoundingRect();
    QRectF newSceneRec = QRectF(itemsRec.x() - 100,
                                itemsRec.y() - 100,
                                itemsRec.width() + 200,
                                itemsRec.height() + 200);

    //setSceneRect(newSceneRec);
    //fitInView(newSceneRec, Qt::KeepAspectRatio);
    setSceneRect(itemsRec);
    fitInView(itemsRec, Qt::KeepAspectRatio);
}


/**
 * @brief NodeView::view_addComponentDefinition
 * Add a ComponentInstance of Component itm into the currently selected ComponentAssembly.
 * @param itm
 */
void NodeView::view_addComponentDefinition(NodeItem *itm)
{
    ComponentAssembly* assm = dynamic_cast<ComponentAssembly*>(controller->getSelectedNode());
    if (assm) {
        Component* defn = dynamic_cast<Component*>(itm->getGraphML());
        if (defn) {
            controller->view_ConstructComponentInstanceInAssembly(defn, assm);
            // sort component assembly
            //view_SortNode(assm);
        }
    }
}


/**
 * @brief NodeView::view_updateDockButtons
 * This method enable/disable dock buttons depending on the currently selected node.
 */
void NodeView::updateDockButtons(Node* node)
{
    if (node) {

        QString nodeKind = node->getDataValue("kind");

        if (nodeKind == "ComponentAssembly") {

            emit updateDockButtons("D");

        } else  if (nodeKind == "InterfaceDefinitions"||
                    nodeKind == "BehaviourDefinitions" ||
                    nodeKind == "AssemblyDefinitions" ||
                    nodeKind == "File" ||
                    nodeKind == "Component") {

            emit updateDockButtons("P");

        } else if (nodeKind == "HardwareDefinitions" ||
                   nodeKind == "HardwareCluster") {

            emit updateDockButtons("H");

        } else if (nodeKind == "ComponentImpl") {

            emit updateDockButtons("PD");

        } else if (nodeKind == "DeploymentDefinitions" ||
                   nodeKind == "ManagementComponent") {

            emit updateDockButtons("N");

        } else {
            emit updateDockButtons("A");
        }
    }
}


/**
 * @brief NodeView::resetSceneRect
 * This method resets the scene rectangle everytime the model is sorted.
 * @param nodeItem
 */
void NodeView::resetSceneRect(NodeItem *nodeItem)
{
    if (nodeItem) {
        QRect* rec = new QRect(0, 0,
                               nodeItem->boundingRect().width()*1.5,
                               nodeItem->boundingRect().height()*1.5);
        setSceneRect(*rec);
        nodeItem->setPos((rec->width()/2) - (nodeItem->boundingRect().width()/2),
                         (rec->height()/2) - (nodeItem->boundingRect().height()/2));
    }
}
