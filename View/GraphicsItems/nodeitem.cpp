#include "nodeitem.h"
#include "edgeitem.h"
#include "../nodeview.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QBitmap>

#include <QInputDialog>
#include <QTextBlockFormat>

#include <QTextCursor>
#include <cmath>

//#define MODEL_WIDTH 66
//#define MODEL_HEIGHT 66

#define MODEL_WIDTH 72
#define MODEL_HEIGHT 72

//#define MARGIN_RATIO 0.10 //NORMAL
#define MARGIN_RATIO (1.0 / 18.0) //NORMAL
//#define ICON_RATIO 0.80 //LARGE


#define ICON_RATIO (4.5 / 6.0) //LARGE
//#define ICON_RATIO (5.0 / 6.0) //LARGE
//#define ICON_RATIO (3.0 / 4.0) //LARGE
#define SMALL_ICON_RATIO ((1.0 / 6.0))
#define TOP_LABEL_RATIO (1.0 / 6.0)
#define RIGHT_LABEL_RATIO (1.5 / 6.0)
#define BOTTOM_LABEL_RATIO (1.0 / 9.0)
#define LABEL_RATIO (1 - ICON_RATIO)



#define SELECTED_LINE_WIDTH 3
#define ITEM_SIZE 72
#define ASPECT_ROW_COUNT 6
#define ASPECT_COL_COUNT 4

#define ACTUAL_ITEM_SIZE (ITEM_SIZE + ((MARGIN_RATIO * 2) * ITEM_SIZE))
#define GRID_COUNT 2
#define GRID_PADDING_RATIO .25

#define GRID_PADDING_SIZE (GRID_PADDING_RATIO * ACTUAL_ITEM_SIZE)
#define GRID_SIZE ((ACTUAL_ITEM_SIZE + GRID_PADDING_SIZE) / GRID_COUNT)

#define SNAP_PERCENTAGE .5


#define ALL 0
#define CONNECTED 1
#define UNCONNECTED 2

#define THEME_LIGHT 0
#define THEME_DARK 1

#define THEME_DARK_NEUTRAL 10
#define THEME_DARK_COLOURED 11


/**
 * @brief NodeItem::NodeItem
 * @param node
 * @param parent
 * @param aspects
 * @param IN_SUBVIEW
 */
NodeItem::NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW):  GraphMLItem(node, GraphMLItem::NODE_ITEM)
{
    Q_INIT_RESOURCE(resources);
    setParentItem(parent);

    parentNodeItem = parent;
    hasEditData = false;


    //Cache on Graphics card! May Artifact.
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    nodeHardwareLocalHost = false;
    nodeMemberIsKey = false;

    //Setup initial states
    isNodeExpanded = true;
    gotVisibleChildren = false;
    canNodeBeConnected = false;
    isNodeOnGrid = false;
    isNodeSorted = false;
    isGridVisible = false;

    isInSubView = IN_SUBVIEW;

    aspectPos = AP_NONE;

    sortTriggerAction = true;
    eventFromMenu = true;
    hasIcon = true;

    showDeploymentWarningIcon = false;
    hasHardwareWarning = false;
    LOCKED_POSITION = false;
    GRIDLINES_ON = false;
    hidden = false;

    HAS_DEFINITION = false;
    IS_IMPL_OR_INST = false;

    editableDataKey = "type";

    if(node){
        IS_IMPL_OR_INST = node->isInstance() || node->isImpl();
    }

    currentResizeMode = NodeItem::NO_RESIZE;

    childrenViewOptionMenu = 0;

    //Setup labels.
    nodeLabel = "";
    nodeType = "";

    topLabelInputItem = 0;
    rightLabelInputItem = 0;
    bottomInputItem = 0;

    //Setup initial sizes
    width = 0;
    height = 0;
    expandedWidth = 0;
    expandedHeight = 0;
    minimumWidth = 0;
    minimumHeight = 0;

    nodeKind = getGraphML()->getDataValue("kind");

    IS_HARDWARE_CLUSTER = (nodeKind == "HardwareCluster");
    IS_MODEL = (nodeKind == "Model");
    IS_DEFINITION = false;

    if(isInSubView){
        CHILDREN_VIEW_MODE = ALL;
    }else{
        CHILDREN_VIEW_MODE = CONNECTED;
    }

    IS_DEFINITION =  node->isAspect();

    if(IS_DEFINITION || IS_MODEL){
        hasIcon = false;
    }




    if(IS_DEFINITION){
        nodeLabel = nodeKind;

        if(nodeKind.startsWith("Interface")){
            aspectPos = AP_TOPLEFT;
            nodeLabel = "INTERFACES";
        }else if(nodeKind.startsWith("Behaviour")){
            aspectPos = AP_TOPRIGHT;
            nodeLabel = "BEHAVIOUR";
        }else if(nodeKind.startsWith("Hardware")){
            aspectPos = AP_BOTRIGHT;
            nodeLabel = "HARDWARE";
        }else if(nodeKind.startsWith("Assembly")){
            aspectPos = AP_BOTLEFT;
            nodeLabel = "ASSEMBLIES";
        }

        aspectLockPos = parent->getAspectsLockedPoint(aspectPos);
        setPos(aspectLockPos);
    }

    QString parentNodeKind = "";
    if (parent) {
        setVisibility(parent->isExpanded());

        setWidth(parent->getChildWidth());
        setHeight(parent->getChildHeight());

        parentNodeKind = parent->getGraphMLDataValue("kind");

    } else {
        setWidth(MODEL_WIDTH);
        setHeight(MODEL_HEIGHT);
    }

    //Set Minimum Size.
    minimumWidth = width;
    minimumHeight = height;

    //Set Maximum Size
    expandedWidth = width;
    expandedHeight = height;

    canNodeBeExpanded = !(IS_DEFINITION || isModel());

    setupLabel();




    setupGraphMLDataConnections();
    updateGraphMLData();

    //Update Width and Height with values from the GraphML Model If they have them.
    //retrieveGraphMLData();

    setupAspect();
    setupBrushes();

    if (IS_HARDWARE_CLUSTER) {
        setupChildrenViewOptionMenu();
    }

    if(IN_SUBVIEW){
        setVisibility(true);
    }else{
        updateModelData();
    }

    if(getParentNodeItem()){
        //getParentNodeItem()->childPosUpdated();
        getParentNodeItem()->updateModelSize();
    }

    aspectsChanged(aspects);

    //darkThemeType = THEME_DARK_NEUTRAL;
    //darkThemeType = THEME_DARK_COLOURED;
    darkThemeType = 0;
}


/**
 * @brief NodeItem::~NodeItem
 */
NodeItem::~NodeItem()
{
    currentLeftEdgeIDs.clear();
    currentRightEdgeIDs.clear();
    if(getNodeView()){
        if(!getNodeView()->isTerminating()){
            if(parentNodeItem){
                //Only unstitch if we aren't terminating
                parentNodeItem->removeChildNodeItem(getID());
            }
        }
    }

    delete rightLabelInputItem;
    delete topLabelInputItem;
    delete bottomInputItem;
}

/**
 * @brief NodeItem::getMouseOverType Returns the type of the node the position
 * @param scenePos - The scene coordinate to check.
 * @return - The type the
 */
NodeItem::MOUSEOVER_TYPE NodeItem::getMouseOverType(QPointF scenePos)
{
    QPointF itemPos = mapFromScene(scenePos);

    //Most mouse over states are only useful when things are actually rendered!
    RENDER_STATE state = getRenderState();


    if(contains(itemPos)){
        if(mouseOverBotInput(itemPos) && state > RS_REDUCED){
            return MO_BOT_LABEL;
        }
        if(mouseOverRightLabel(itemPos) && state >= RS_REDUCED){
            return MO_EXPANDLABEL;
        }
        if(mouseOverConnect(itemPos) && state > RS_REDUCED){
            return MO_CONNECT;
        }
        if(isModel()){
            if(mouseOverModelCircle(itemPos)){
                return MO_MODEL_CIRCLE;
            }
            if(mouseOverModelQuadrant(itemPos)){
                if(mouseOverModelBL(itemPos)){
                    return MO_MODEL_BL;
                }else if(mouseOverModelBR(itemPos)){
                    return MO_MODEL_BR;
                }else if(mouseOverModelTR(itemPos)){
                    return MO_MODEL_TR;
                }else if(mouseOverModelTL(itemPos)){
                    return MO_MODEL_TL;
                }
            }

        }if(mouseOverDefinition(itemPos) && state > RS_REDUCED){
            return MO_DEFINITION;
        }if(mouseOverHardwareMenu(itemPos) && state > RS_REDUCED){
            return MO_HARDWAREMENU;
        }if(mouseOverDeploymentIcon(itemPos) && state > RS_REDUCED){
            return MO_DEPLOYMENTWARNING;
        }if(mouseOverIcon(itemPos)){
            return MO_ICON;
        }if(mouseOverTopBar(itemPos)){
            return MO_TOPBAR;
        }if(isResizeable()){
            NodeItem::RESIZE_TYPE resize = resizeEntered(itemPos);
            if(resize == NO_RESIZE){
                return MO_ITEM;
            }else if(resize == RESIZE){
                return MO_RESIZE;
            }else if(resize == HORIZONTAL_RESIZE){
                return MO_RESIZE_HOR;
            }else if(resize == VERTICAL_RESIZE){
                return MO_RESIZE_VER;
            }
        }else {
            if (!isModel()) {
                return MO_ITEM;
            }
        }
    }
    return MO_NONE;
}

void NodeItem::setEditableField(QString keyName, bool dropDown)
{
    editableDataKey = keyName;
    editableDataDropDown = dropDown;


    connectToGraphMLData(keyName);

    if(bottomInputItem){
        bottomInputItem->setDropDown(dropDown);

        if(keyName.isEmpty()){
            bottomInputItem->setVisible(false);
        }else{
            hasEditData = true;
            bottomInputItem->setVisible(true);
        }
    }

    if(getGraphML()){
        graphMLDataChanged(getGraphML()->getData(keyName));
    }


}

/**
 * @brief NodeItem::setZValue Overides the QGraphicsItem::setZValue function to recurse up it's parentNodeItem and set the Z-Value on its parents.
 * @param z The ZValue to set.
 */
void NodeItem::setZValue(qreal z)
{
    oldZValue = zValue();
    if(parentNodeItem){
        parentNodeItem->setZValue(z);
    }
    QGraphicsItem::setZValue(z);
}

/**
 * @brief NodeItem::restoreZValue Resets the Z-Value back to it's original Z-Value (Calls SetZValue())
 */
void NodeItem::restoreZValue()
{
    QGraphicsItem::setZValue(oldZValue);
    if(parentNodeItem){
        parentNodeItem->restoreZValue();
    }
}

/**
 * @brief NodeItem::setNodeConnectable Sets whether or not this node has a visible icon to allow connections to be "drawn"
 * @param connectable Is this node able to connect visually.
 */
void NodeItem::setNodeConnectable(bool connectable)
{
    if(connectable != canNodeBeConnected){
        canNodeBeConnected = connectable;
        update();
    }
}

void NodeItem::childHidden()
{
    //Update!
    gotVisibleChildren = hasVisibleChildren();
}

QColor NodeItem::getBackgroundColor()
{
    return headerBrush.color();
}


/**
 * @brief NodeItem::setVisibleParentForEdgeItem
 * @param line
 * @param RIGH
 * @return The index of the
 */
void NodeItem::setVisibleParentForEdgeItem(int ID, bool RIGHT)
{
    if(RIGHT){
        if(!currentRightEdgeIDs.contains(ID)){
            currentRightEdgeIDs.append(ID);
            if(currentRightEdgeIDs.size() > 1){
                //emit NodeItem_Moved();
            }
        }
    }else{
        if(!currentLeftEdgeIDs.contains(ID)){
            currentLeftEdgeIDs.append(ID);
            if(currentLeftEdgeIDs.size() > 1){
                //emit NodeItem_Moved();
            }
        }

    }
}

int NodeItem::getIndexOfEdgeItem(int ID, bool RIGHT)
{
    int id = -1;
    if(RIGHT){
        if(currentRightEdgeIDs.contains(ID)){
            id =  currentRightEdgeIDs.indexOf(ID);
        }
    }else{
        if(currentLeftEdgeIDs.contains(ID)){
            id = currentLeftEdgeIDs.indexOf(ID);
        }
    }
    return id;
}

int NodeItem::getNumberOfEdgeItems(bool RIGHT)
{
    if(RIGHT){
        return currentRightEdgeIDs.count();
    }else{
        return currentLeftEdgeIDs.count();
    }
}

void NodeItem::removeVisibleParentForEdgeItem(int ID)
{
    if(!isDeleting()){
        currentRightEdgeIDs.removeOne(ID);
        currentLeftEdgeIDs.removeOne(ID);
    }
}


NodeItem *NodeItem::getParentNodeItem()
{
    return dynamic_cast<NodeItem*>(parentItem());
}


QList<EdgeItem *> NodeItem::getEdgeItems()
{
    return this->connections;
}


void NodeItem::setParentItem(QGraphicsItem *parent)
{
    NodeItem* parentItem = dynamic_cast<NodeItem*>(parent);
    if(parentItem){
        parentItem->addChildNodeItem(this);

        if(!parentItem->isModel()){
            connect(parentItem, SIGNAL(NodeItem_Moved()), this, SIGNAL(NodeItem_Moved()));
            connect(this, SIGNAL(NodeItem_Moved()), parentItem, SLOT(childMoved()));
        }

    }
    QGraphicsItem::setParentItem(parent);
}


QRectF NodeItem::boundingRect() const
{
    qreal top = 0;
    qreal bot = 0;
    qreal left = 0;
    qreal right = 0;

    if(IS_MODEL){
        //Centered on 0,0
        left  = - (width / 2.0);
        right = + (width / 2.0);
        top   = - (height / 2.0);
        bot   = + (height / 2.0);
    }else{
        //Top left Justified.
        float itemMargin = getItemMargin() * 2;
        right = width + itemMargin;
        bot = height + itemMargin;
    }
    return QRectF(QPointF(left, top), QPointF(right, bot));
}

QRectF NodeItem::childrenBoundingRect() const
{
    QRectF rect;

    foreach(NodeItem* child, childNodeItems){
        QRectF childRect = child->boundingRect();
        childRect.translate(child->pos());
        rect = rect.united(childRect);
    }
    return rect;
}


QRectF NodeItem::minimumBoundingRect() const
{
    if(IS_MODEL){
        return boundingRect();
    }else{
        qreal itemMargin = getItemMargin() * 2;
        return QRectF(QPointF(0, 0), QPointF(minimumWidth + itemMargin, minimumHeight + itemMargin));
    }
}

QRectF NodeItem::expandedBoundingRect() const
{
    if(IS_MODEL){
        return boundingRect();
    }else{
        qreal itemMargin = getItemMargin() * 2;

        return QRectF(QPointF(0, 0), QPointF(expandedWidth + itemMargin, expandedHeight + itemMargin));
        //return QRectF(QPointF(getItemMargin(), getItemMargin()), QPointF(expandedWidth + getItemMargin(), expandedHeight + getItemMargin()));
    }
}

QRectF NodeItem::expandedLabelRect() const
{
    qreal itemMargin = getItemMargin();

    qreal right = expandedWidth + itemMargin;
    qreal left = itemMargin + minimumWidth;
    qreal top = (itemMargin * 2 + minimumHeight) /2;

    return QRectF(QPointF(left, top), QPointF(right, top));
}



int NodeItem::getEdgeItemIndex(EdgeItem *item)
{
    return connections.indexOf(item);

}

int NodeItem::getEdgeItemCount()
{
    return connections.size();


}

/**
 * @brief NodeItem::gridRect Returns a QRectF which contains the local coordinates of where the Grid lines are to be drawn.
 * @return The grid rectangle
 */
QRectF NodeItem::gridRect()
{
    qreal itemMargin = getItemMargin();

    QPointF topLeft(itemMargin, itemMargin);
    QPointF bottomRight = topLeft + QPointF(expandedWidth, expandedHeight);

    //If it has an icon, Start the grid at the next grid line.
    if(hasIcon){
        int roundedGridCount = ceil(minimumHeight / getGridSize());
        topLeft.setY(roundedGridCount * getGridSize() - getGridGapSize()/2);
    }


    //Enforce at least one grid in height. Size!
    qreal deltaY = bottomRight.y() - topLeft.y();
    if(deltaY <= 0){
        deltaY = qMax(deltaY, getGridSize());
        bottomRight.setY(bottomRight.y() + deltaY);
    }

    return QRectF(topLeft, bottomRight);
}

QPointF NodeItem::getCenterOffset()
{
    return QPointF((width / 2) + getItemMargin(), (height / 2) + getItemMargin());
}

QRectF NodeItem::headerRect()
{
    qreal itemMargin = 2 * getItemMargin();
    return QRectF(QPointF(0, 0), QPointF(width + itemMargin, minimumHeight + itemMargin));
}

QRectF NodeItem::bodyRect()
{
    qreal itemMargin = 2 * getItemMargin();
    return QRectF(QPointF(0, minimumHeight + itemMargin), QPointF(width + itemMargin, height + itemMargin));
}

QRectF NodeItem::getChildBoundingRect()
{
    return QRectF(QPointF(0, 0), QPointF(getChildWidth() + (2* getChildItemMargin()), getChildHeight() + (2 * getChildItemMargin())));
}


QPointF NodeItem::getGridPosition(int x, int y)
{
    QPointF topLeft = gridRect().topLeft();
    topLeft.setX(topLeft.x() + (x * getGridSize()));
    topLeft.setY(topLeft.y() + (y * getGridSize()));
    return topLeft;
}

bool NodeItem::isHardwareHighlighted()
{
    return hasHardwareWarning;
}

void NodeItem::setHardwareHighlighting(bool highlighted)
{
    hasHardwareWarning = highlighted;
    update();
}


bool NodeItem::isLocked()
{
    return LOCKED_POSITION;
}

bool NodeItem::isModel()
{
    return IS_MODEL;
}

bool NodeItem::isAspect()
{
    return IS_DEFINITION;
}


/**
 * @brief NodeItem::isHardwareCluster
 * Return if this nodeitem is a HarwareCluster or not.
 * @return
 */
bool NodeItem::isHardwareCluster()
{
    return IS_HARDWARE_CLUSTER;
}

void NodeItem::setLocked(bool locked)
{
    LOCKED_POSITION = locked;
}




void NodeItem::addChildNodeItem(NodeItem *child)
{
    if(!childNodeItems.contains(child->getID())){
        childNodeItems.insert(child->getID(), child);
        childrenIDs.append(child->getID());

        if(!child->isHidden()){
            gotVisibleChildren = true;
        }
    }
}


void NodeItem::removeChildNodeItem(int ID)
{
    childrenIDs.removeAll(ID);
    childNodeItems.remove(ID);
    removeChildOutline(ID);
    if(childNodeItems.size() == 0){
        prepareGeometryChange();
        resetSize();
        gotVisibleChildren = false;
    }else{
        gotVisibleChildren = hasVisibleChildren();
    }
}





void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget)
{
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    //Get Render State.
    RENDER_STATE renderState = getRenderState();

    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();

    if(isModel()){
        paintModel2(painter);
        return;
    }

    //Background
    if(renderState > RS_NONE){
        QBrush bodyBrush = this->bodyBrush;
        QBrush headBrush = this->headerBrush;

        //Make the background transparent
        if(viewState == NodeView::VS_MOVING || viewState == NodeView::VS_RESIZING){
            if(isSelected() && isNodeOnGrid){
                QColor color = bodyBrush.color();
                color.setAlpha(90);
                bodyBrush.setColor(color);
            }
        }


        //Paint Background
        painter->setPen(Qt::NoPen);
        painter->setBrush(bodyBrush);
        painter->drawRect(boundingRect());

        if(IS_IMPL_OR_INST && !HAS_DEFINITION){
            //Change Brush style.
            headBrush.setStyle(Qt::BDiagPattern);
        }

        if(isHighlighted()){
            headBrush.setColor(Qt::white);
        }


        if(!IS_DEFINITION){
            //Paint Header
            painter->setPen(Qt::NoPen);
            painter->setBrush(headBrush);
            painter->drawRect(headerRect());
        }


        //Draw the boundary.
        if(renderState >= RS_REDUCED || isSelected() || hasHardwareWarning){
            //Setup the Pen
            QPen pen = this->pen;

            if(isSelected()){
                pen = this->selectedPen;
                pen.setWidthF(selectedPenWidth);
            }else{
                if(IS_DEFINITION){
                    pen = Qt::NoPen;
                }
            }

            if(hasHardwareWarning){
                //Trace the boundary
                pen.setStyle(Qt::DotLine);
                pen.setColor(Qt::red);
            }

            if(isHovered()){
                if(pen.color() == Qt::black){
                    pen.setColor(QColor(120,120,120));
                }else{
                    pen.setColor(pen.color().lighter());
                }
            }

            //Trace the boundary
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);

            painter->drawRect(adjustRectForBorder(boundingRect(), pen.widthF()));
        }

        if(IS_DEFINITION){
            QPen fontPen;
            fontPen.setColor(bodyBrush.color().darker(110));

            QFont definitionFont;
            definitionFont.setBold(true);
            definitionFont.setPointSize(minimumHeight * (LABEL_RATIO /2));
            definitionFont.setWeight(QFont::Black);

            painter->setPen(fontPen);
            painter->setFont(definitionFont);
            painter->drawText(boundingRect(), Qt::AlignHCenter | Qt::AlignBottom, nodeLabel);
        }



        //New Code
        if(drawGridlines()){
            painter->setClipping(false);

            painter->setPen(Qt::gray);
            QPen linePen = painter->pen();

            linePen.setStyle(Qt::DotLine);
            //linePen.setWidth(1);
            painter->setPen(linePen);

            painter->drawLines(xGridLines);
            painter->drawLines(yGridLines);
        }

        //Paint the Icon
        if(hasIcon){
            paintPixmap(painter, IP_CENTER, "Items", getIconURL());
        }
    }



    if(renderState == RS_FULL){
        //If a Node has a Definition, paint a definition icon
        if(HAS_DEFINITION){
            paintPixmap(painter, IP_TOPLEFT, "Actions", "Definition");
        }else if (nodeKind == "HardwareCluster") {
            paintPixmap(painter, IP_TOPLEFT, "Actions", "MenuCluster");
        }

        if(nodeMemberIsKey){
            paintPixmap(painter, IP_TOPLEFT, "Actions", "Key");
        }

        if(canNodeBeConnected){
            //Paint connect Icon
            paintPixmap(painter, IP_TOPRIGHT, "Actions", "ConnectTo");

            QPen newPen(Qt::gray);
            newPen.setWidthF(0.5);
            painter->setPen(newPen);
            painter->drawEllipse(iconRect_TopRight());
        }

        if(canNodeBeExpanded){
            if(gotVisibleChildren){
                if(isExpanded()){
                    paintPixmap(painter, IP_BOT_RIGHT, "Actions", "Contract");
                }else{
                    paintPixmap(painter, IP_BOT_RIGHT, "Actions", "Expand");
                }
            }
        }



        if(hasEditData){
            paintPixmap(painter, IP_BOTLEFT, "Data", editableDataKey);
        }
    }

    if(renderState >= RS_REDUCED){
        //If this Node has a Deployment Warning, paint a warning Icon
        if(showDeploymentWarningIcon){
            paintPixmap(painter, IP_TOPRIGHT, "Actions", "Warning");
        }
    }
}

void NodeItem::paintModel(QPainter *painter)
{
    painter->setPen(Qt::NoPen);


    if(true){
        //CLIP ON CIRCLE
        QPainterPath clipPath;
        clipPath.addEllipse(boundingRect());
        painter->setClipping(true);
        painter->setClipPath(clipPath);
    }

    QPointF origin(0,0);

    QRectF quadrant(0, 0, width / 2, height / 2);

    //Paint Bottom Right
    //painter->setBrush(QColor(110,170,220));
    painter->setBrush(bottomRightColor);
    painter->drawRect(quadrant);

    //Paint Top Right
    quadrant.moveBottomLeft(origin);
    //painter->setBrush(QColor(254,184,126));
    painter->setBrush(topRightColor);
    painter->drawRect(quadrant);

    //Paint Top Left
    quadrant.moveBottomRight(origin);
    //painter->setBrush(QColor(110,210,210));
    painter->setBrush(topLeftColor);
    painter->drawRect(quadrant);

    //Paint Bottom Left
    quadrant.moveTopRight(origin);
    //painter->setBrush(QColor(255,160,160));
    painter->setBrush(bottomLeftColor);
    painter->drawRect(quadrant);

    quadrant.moveCenter(origin);



    if(getRenderState() >= RS_REDUCED){
        //Draw the intersection lines.
        painter->setPen(Qt::gray);
        painter->drawLine(QPointF(0, -height / 2), QPointF(0, height / 2));
        painter->drawLine(QPointF(-width / 2, 0), QPointF(width / 2, 0));
    }


    //Setup the Pen
    QPen pen = this->pen;

    painter->setPen(pen);

    if(isSelected()){
        pen = this->selectedPen;
        pen.setWidthF(selectedPenWidth);
    }

    painter->setPen(pen);
    painter->setBrush(modelCircleColor);
    //Draw the center circle
    painter->drawEllipse(quadrant);


}

void NodeItem::paintModel2(QPainter *painter)
{
    painter->setPen(Qt::NoPen);


    QRectF totalRect = QRectF(-width/2, -height/2, width, height);
    if(true){
        //CLIP ON CIRCLE
        //QPainterPath clipPath;
        //clipPath.addEllipse(boundingRect());
        //painter->setClipping(true);
        //painter->setClipPath(clipPath);

         painter->setPen(Qt::black);
         painter->drawRect(totalRect);
         painter->setPen(Qt::NoPen);
    }


    QPointF origin(0,0);

    QRectF quadrant(0, 0, width / 2, height / 2);

    //Paint Bottom Right
    //painter->setBrush(QColor(110,170,220));
    quadrant.moveBottomRight(totalRect.bottomRight());
    painter->setBrush(bottomRightColor);
    painter->drawRect(quadrant);

    //Paint Top Right
    quadrant.moveTopRight(totalRect.topRight());
    //painter->setBrush(QColor(254,184,126));
    painter->setBrush(topRightColor);
    painter->drawRect(quadrant);

    //Paint Top Left
    quadrant.moveTopLeft(totalRect.topLeft());
    //painter->setBrush(QColor(110,210,210));
    painter->setBrush(topLeftColor);
    painter->drawRect(quadrant);

    //Paint Bottom Left
    quadrant.moveBottomLeft(totalRect.bottomLeft());
    //painter->setBrush(QColor(255,160,160));
    painter->setBrush(bottomLeftColor);
    painter->drawRect(quadrant);

    quadrant.setWidth(width/1.5);
    quadrant.setHeight(width/1.5);
    quadrant.moveCenter(origin);



    if(getRenderState() >= RS_REDUCED){
        //Draw the intersection lines.
        painter->setPen(Qt::gray);
        painter->drawLine(QPointF(0, -height / 2), QPointF(0, height / 2));
        painter->drawLine(QPointF(-width / 2, 0), QPointF(width / 2, 0));
    }


    //Setup the Pen
    QPen pen = this->pen;

    painter->setPen(pen);

    if(isSelected()){
        pen = this->selectedPen;
        pen.setWidthF(selectedPenWidth);
    }

    painter->setPen(pen);
    painter->setBrush(modelCircleColor);
    //Draw the center circle
    painter->drawRect(quadrant);

}


bool NodeItem::hasVisibleChildren()
{
    if(IS_DEFINITION){
        return true;
    }
    foreach(NodeItem* child, childNodeItems){
        if(!child->isHidden()){
            return true;
        }
    }
    return false;
}

bool NodeItem::mouseOverModelCircle(QPointF mousePosition)
{
    if(isModel()){
        QLineF distance = QLineF(mousePosition, QPointF(0,0));
        if(distance.length() < (width / 4)){
            return true;
        }
    }
    return false;
}

bool NodeItem::mouseOverModelQuadrant(QPointF mousePosition)
{
    if(isModel()){
        QLineF distance = QLineF(mousePosition, QPointF(0,0));
        if(distance.length() > (width / 4) && distance.length() < (width/2)){
            return true;
        }
    }
    return false;
}

bool NodeItem::mouseOverModelTR(QPointF mousePosition)
{
    if(isModel() && mouseOverModelQuadrant(mousePosition)){
        if(mousePosition.x() > 0 && mousePosition.y() < 0){
            return true;
        }
    }
    return false;
}

bool NodeItem::mouseOverModelBR(QPointF mousePosition)
{
    if(isModel() && mouseOverModelQuadrant(mousePosition)){
        if(mousePosition.x() > 0 && mousePosition.y() > 0){
            return true;
        }
    }
    return false;

}

bool NodeItem::mouseOverModelBL(QPointF mousePosition)
{
    if(isModel() && mouseOverModelQuadrant(mousePosition)){
        if(mousePosition.x() < 0 && mousePosition.y() > 0){
            return true;
        }
    }
    return false;
}

bool NodeItem::mouseOverModelTL(QPointF mousePosition)
{
    if(isModel() && mouseOverModelQuadrant(mousePosition)){
        if(mousePosition.x() < 0 && mousePosition.y() < 0){
            return true;
        }
    }
    return false;

}


bool NodeItem::mouseOverRightLabel(QPointF mousePosition)
{
    if(rightLabelInputItem && rightLabelInputItem->isVisible()){
        QRectF labelRect = rightLabelInputItem->boundingRect();
        labelRect.translate(rightLabelInputItem->pos());
        if(labelRect.contains(mousePosition)){
            return true;
        }
    }
    return false;
}


bool NodeItem::mouseOverBotInput(QPointF mousePosition)
{
    if(bottomInputItem && bottomInputItem->isVisible()){
        QRectF labelRect = bottomInputItem->boundingRect();
        labelRect.translate(bottomInputItem->pos());
        if(labelRect.contains(mousePosition)){
            return true;
        }
    }
    return false;

}

bool NodeItem::mouseOverDeploymentIcon(QPointF mousePosition)
{
    if(showDeploymentWarningIcon){
        return iconRect_TopRight().contains(mousePosition);
    }
    return false;
}

bool NodeItem::mouseOverDefinition(QPointF mousePosition)
{
    if (HAS_DEFINITION || nodeMemberIsKey){
        return iconRect_TopLeft().contains(mousePosition);
    }
    return false;
}

bool NodeItem:: mouseOverIcon(QPointF mousePosition)
{
    if(hasIcon){
        if(iconRect().contains(mousePosition)){
            return true;
        }
    }
    return false;
}

bool NodeItem::mouseOverTopBar(QPointF mousePosition)
{
    if(isModel() || IS_DEFINITION){
        return false;
    }else{
        return headerRect().contains(mousePosition);
    }
}


/**
 * @brief NodeItem::menuArrowPressed
 * @param mousePosition
 * @return
 */
bool NodeItem::mouseOverHardwareMenu(QPointF mousePosition)
{
    if (IS_HARDWARE_CLUSTER) {
        if(iconRect_TopLeft().contains(mousePosition)){
            return true;
        }
    }
    return false;
}

bool NodeItem::mouseOverConnect(QPointF mousePosition)
{
    if(canNodeBeConnected){
        return iconRect_TopRight().contains(mousePosition);
    }
    return false;
}

bool NodeItem::mouseOverExpand(QPointF mousePosition)
{
    if(canNodeBeExpanded){
        return iconRect_BottomRight().contains(mousePosition);
    }
    return false;
}


NodeItem::RESIZE_TYPE NodeItem::resizeEntered(QPointF mousePosition)
{
    //Check if the Mouse is in the Bottom Right Corner.
    if(iconRect_BottomRight().contains(mousePosition)){
        return RESIZE;
    }

    int cornerRadius = 10;
    //Calculate the Corners for the Horizontal resize
    QPointF topLeft = boundingRect().topRight() + QPointF(-cornerRadius, cornerRadius);
    QPointF bottomRight = boundingRect().bottomRight() - QPointF(0, cornerRadius);

    //Check if the mouse is contained in the rectangle on the right of the NodeItem.
    QRectF horizontalResizeRectangle = QRectF(topLeft, bottomRight);
    if(horizontalResizeRectangle.contains(mousePosition)){
        return HORIZONTAL_RESIZE;
    }

    //Calculate the Corners for the Vertical resize
    QPointF bottomLeft = boundingRect().bottomLeft() + QPointF(cornerRadius, -cornerRadius);
    bottomRight = boundingRect().bottomRight() + QPointF(-cornerRadius, 0);

    //Check if the mouse is contained in the rectangle on the bottom of the NodeItem.
    QRectF verticalRect = QRectF(bottomLeft, bottomRight);
    if(verticalRect.contains(mousePosition)){
        return VERTICAL_RESIZE;
    }

    return NO_RESIZE;
}

void NodeItem::addChildEdgeItem(EdgeItem *edge)
{
    if(!childEdges.contains(edge)){
        childEdges.append(edge);
    }
}

QList<EdgeItem *> NodeItem::getChildEdges()
{
    return childEdges;
}

void NodeItem::removeChildEdgeItem(EdgeItem *edge)
{
    childEdges.removeAll(edge);
}

/**
 * @brief NodeItem::isHidden
 * @return
 */
bool NodeItem::isHidden()
{
    return hidden;
}


double NodeItem::getWidth()
{
    return width;

}


double NodeItem::getHeight()
{
    return height;
}

void NodeItem::updateDefinition(){
    if(IS_IMPL_OR_INST){
        if(getNodeView()){
            int defID = getNodeView()->getDefinitionID(getID());
            bool hasDef = defID != -1;
            if(HAS_DEFINITION != hasDef){
                HAS_DEFINITION = hasDef;
                update();
            }
        }
    }
}

void NodeItem::addEdgeItem(EdgeItem *line)
{
    connections.append(line);
}


void NodeItem::removeEdgeItem(EdgeItem *line)
{
    connections.removeAll(line);
}


void NodeItem::setCenterPos(QPointF pos)
{
    //pos is the new center Position.
    pos -= minimumBoundingRect().center();
    //QGraphicsItem::setPos(pos);

    setPos(pos);
}

QPointF NodeItem::centerPos()
{
    return pos() + minimumBoundingRect().center();
}


void NodeItem::adjustPos(QPointF delta)
{
    setLocked(false);

    QPointF currentPos = pos();
    currentPos += delta;
    //hasSelectionMoved = true;
    setPos(currentPos);
}

void NodeItem::adjustSize(QSizeF delta)
{
    qreal newWidth = getWidth() + delta.width();
    qreal newHeight = getHeight() + delta.height();

    setWidth(newWidth);
    setHeight(newHeight);
}


void NodeItem::addChildOutline(NodeItem *nodeItem, QPointF gridPoint)
{
    prepareGeometryChange();
    QRectF nodeRect = nodeItem->boundingRect();
    QPointF deltaPos = gridPoint - nodeItem->minimumBoundingRect().center();
    nodeRect.translate(deltaPos);
    outlineMap.insert(nodeItem->getID(), nodeRect);
}

void NodeItem::removeChildOutline(int ID)
{
    if(outlineMap.contains(ID)){
        prepareGeometryChange();
        outlineMap.remove(ID);
    }
}



double NodeItem::getChildWidth()
{
    // added an offset of 0.35 here and in getChildHeight
    // to make the gap between the view aspects uniform
    if (isModel()) {
        return GRID_SIZE * GRID_COUNT * ASPECT_ROW_COUNT;
    }
    return ITEM_SIZE;
}

double NodeItem::getChildHeight()
{
    if (isModel()) {
        return GRID_SIZE * GRID_COUNT * ASPECT_COL_COUNT;
    }
    return ITEM_SIZE;
}


/**
 * @brief NodeItem::getNextChildPos
 * @return
 */
QPointF NodeItem::getNextChildPos(QRectF itemRect, bool currentlySorting)
{
    bool useItemRect = !itemRect.isNull();

    QPainterPath childrenPath = QPainterPath();
    //bool hasChildren = false;


    // add the children's bounding rectangles to the children path
    foreach(NodeItem* child, getChildNodeItems()){
        if(child && child->isInAspect() && !child->isHidden()){
            if(!currentlySorting || (currentlySorting && child->isSorted())){
                //hasChildren = true;
                QRectF childRect =  child->boundingRect();
                childRect.translate(child->pos());
                childrenPath.addRect(childRect);
            }
        }
    }

    // CATHLYNS CODE FOR THE SAME THING. TEST
    // work out how many grid cells are needed per child item
    // divide it by 2 - only need half the number of cells to fit the center of the item
    double startingGridPoint = ceil(getChildBoundingRect().width()/getGridSize()) / 2;
    double currentX = startingGridPoint;
    double currentY = startingGridPoint;

    double maxX = 0;
    bool xOutsideOfGrid = false;
    bool yOutsideOfGrid = false;

    while (true) {

        // get the next position; construct a child rect at that position
        QPointF nextPosition = getGridPosition(currentX, currentY);
        QRectF childRect = getChildBoundingRect();
        if(useItemRect){
            childRect = itemRect;
            childRect.translate(nextPosition - getChildBoundingRect().center());
        }else{
            //Translate to the center of the normal childBR
            childRect.translate(nextPosition - childRect.center());
        }

        // check if the child rect collides with an existing child item
        if (childrenPath.intersects(childRect)) {
            // if so, check the next x position
            currentX++;
            // collision means that current position is inside the grid
            xOutsideOfGrid = false;
            yOutsideOfGrid = false;
        } else {
            // if there is no collision and the current position is inside the grid
            // it's a valid position - return it
            //if (gridRect().contains(nextPosition)) {
            if (gridRect().contains(childRect)) {
                return nextPosition;
            }

            // if both currentX and currentY are outside of the grid,
            // it means that there is no available spot in the grid
            if (xOutsideOfGrid && yOutsideOfGrid) {
                // return a new position depending on which of the width/height is bigger
                QPointF finalPosition = getGridPosition(maxX, startingGridPoint);
                if (boundingRect().width() > boundingRect().height()) {
                    finalPosition = getGridPosition(startingGridPoint, currentY);
                }
                return finalPosition;
            }

            // because currentY was incremented when currentX was outside
            // of the grid, it means that it is now also out of the grid
            if (xOutsideOfGrid) {
                yOutsideOfGrid = true;
            } else {
                // when it gets into this case, it means that currentX is outside of the grid
                xOutsideOfGrid = true;
                // store the maximum x
                if (currentX > maxX) {
                    maxX = currentX;
                }
                // reset currentX then check the next y position
                currentX = startingGridPoint;
                currentY++;
            }
        }
    }

}





/**
 * @brief NodeItem::setSelected Sets the NodeItem as selected, notifies connected edges to highlight.
 * @param selected
 */
void NodeItem::setSelected(bool selected)
{
    //If the current state of the NodeItem's selection is different to the one provided.
    if(isSelected() != selected){
        if(selected){
            //Bring the item to the front if selected.
            setZValue(zValue() + 1);
        }else{
            //Restore the items previous ZValue.
            restoreZValue();
        }
        //Call the base class.
        GraphMLItem::setSelected(selected);
    }
}

/**
 * @brief NodeItem::setVisibilty Changes the Visibility of the NodeItem also notifies all connected edges.
 * @param visible
 */
void NodeItem::setVisibility(bool visible)
{
    bool emitChange = isVisibleTo(0) != visible;

    QGraphicsItem::setVisible(visible);

    if(isLocked()){
        if(visible){
            //Put it back on the grid.
            this->parentNodeItem->addChildOutline(this, this->centerPos());
        }else{
            //Remove the outline.
            this->parentNodeItem->removeChildOutline(this->getID());
        }
    }




    emit setEdgeVisibility(visible);


    if(emitChange){
        emit visibilityChanged(visible);
    }
}



/**
 * @brief NodeItem::graphMLDataChanged This method is called when any connected GraphMLData object updates their value.
 * @param data The GraphMLData which has been changed.
 */
void NodeItem::graphMLDataChanged(GraphMLData* data)
{

    if(getGraphML() && data && data->getParent() == getGraphML() && !getGraphML()->isDeleting()){
        QString keyName = data->getKeyName();
        QString value = data->getValue();
        bool isDouble = false;
        double valueD = value.toDouble(&isDouble);

        if((keyName == "x" || keyName == "y") && isDouble){
            //If data is related to the position of the NodeItem
            //Get the current center position.
            QPointF oldCenter = centerPos();

            QPointF newCenter = centerPos();

            if(keyName == "x"){
                newCenter.setX(valueD);
            }else if(keyName == "y"){
                newCenter.setY(valueD);
            }

            //Update the center position.
            setCenterPos(newCenter);

            //Check if the X or Y has changed.
            newCenter = centerPos();

            if(keyName == "x" && (newCenter.x() != oldCenter.x())){
                emit GraphMLItem_SetGraphMLData(getID(), "x", QString::number(newCenter.x()));
            }
            if(keyName == "y" && (newCenter.y() != oldCenter.y())){
                emit GraphMLItem_SetGraphMLData(getID(), "y", QString::number(newCenter.y()));
            }

        }else if((keyName == "width" || keyName == "height") && isDouble){
            //If data is related to the size of the NodeItem
            if(keyName == "width"){
                modelWidth = valueD;
                setWidth(valueD);
            }else if(keyName == "height"){
                //If NodeItem is contracted and the new value is bigger than the minimum height.
                bool setExpanded = isContracted() && valueD > minimumHeight;

                //If NodeItem is expanded and the new value is equal to the minimum height (String comparison to ignore sigfigs)
                bool setContracted = isExpanded() && valueD <= minimumHeight;

                if(setExpanded){
                    setNodeExpanded(true);
                }
                if(setContracted){
                    setNodeExpanded(false);
                }
                modelHeight = valueD;

                setHeight(valueD);
            }

            double newWidth = ignoreInsignificantFigures(valueD, width);
            double newHeight = ignoreInsignificantFigures(valueD, height);

            //Check if the Width or Height has changed.
            if(keyName == "width" && newWidth != valueD){
                emit GraphMLItem_SetGraphMLData(getID(), "width", QString::number(newWidth));
            }
            if(keyName == "height" && newHeight != valueD){
                emit GraphMLItem_SetGraphMLData(getID(), "height", QString::number(newHeight));
            }
        }else if(keyName == "label"){
            //Update the Label
            updateTextLabel(value);

            // update connected dock node item
            emit updateDockNodeItem();
        }else if(keyName == "architecture"){
            nodeHardwareArch = value;
            update();
        }else if(keyName == "os"){
            nodeHardwareOS = value;
            update();
        }else if(keyName == "type"){
            this->nodeType = value;
        }else if(keyName == "localhost"){
            this->nodeHardwareLocalHost = value == "true";
            update();
        }else if(keyName == "key"){
            nodeMemberIsKey = value == "true";
            update();
        }

        if(keyName == editableDataKey){
            if(bottomInputItem){
                bottomInputItem->setValue(value);

                if(data->isProtected()){
                    bottomInputItem->setBrush(Qt::NoBrush);
                }else{
                    bottomInputItem->setBrush(Qt::white);
                }
            }
        }
    }
}


/**
 * @brief NodeItem::newSort
 */
void NodeItem::sort()
{
    if(isModel()){
        //sortModel();
        return;
    }

    if (sortTriggerAction) {
        // added this so sort can be un-done
        GraphMLItem_TriggerAction("NodeItem: Sorting Node");
    }

    //Get the number of un-locked items
    QMap<int, NodeItem*> toSortMap;
    QList<NodeItem*> lockedItems;

    foreach(NodeItem* child, getChildNodeItems()){
        Node* childNode = child->getNode();
        if(child->getChildNodeItems().size() == 0){
            //RESET SIZE.
            child->setWidth(getChildWidth());
            child->setHeight(getChildHeight());
        }
        if(!child->isVisible()){ //&& nodeKind != "Model"){
            continue;
        }
        if(child->isLocked() && GRIDLINES_ON){
            child->setSorted(true);
            lockedItems.append(child);
            continue;
        }
        //Treat sorted items as locked items.
        if(child->isSorted() && GRIDLINES_ON){
            lockedItems.append(child);
            continue;
        }

        Node* childParent = childNode->getParentNode();

        int currentSortOffset = 0;
        while(childParent){
            if(childParent == getNode()){
                break;
            }else{
                bool isInt;
                int childParentSortOrder = childParent->getDataValue("sortOrder").toInt(&isInt);
                if(isInt){
                    currentSortOffset += childParentSortOrder;
                }
            }
            childParent = childParent->getParentNode();
        }

        bool isInt;
        int sortPosition = childNode->getDataValue("sortOrder").toInt(&isInt);
        if(isInt){
            toSortMap.insertMulti(currentSortOffset + sortPosition, child);
        }
    }

    QList<NodeItem*> toSortItems = toSortMap.values();

    //Sort Items
    while(toSortItems.size() > 0){
        NodeItem* item = toSortItems.takeFirst();

        item->setCenterPos(getNextChildPos(item->boundingRect(), true));
        item->updateModelPosition();
        item->setSorted(true);
    }

    foreach(NodeItem* child, getChildNodeItems()){
        child->setSorted(false);
    }
}

QPointF NodeItem::getAspectsLockedPoint(ASPECT_POS asPos)
{
    //Calculate the margin between each item
    qreal itemMargin = (width / 4);

    switch(asPos){
    case AP_TOPLEFT:
        return QPointF(-itemMargin, -itemMargin);
    case AP_TOPRIGHT:
        return QPointF(itemMargin, -itemMargin);
    case AP_BOTRIGHT:
        return QPointF(itemMargin, itemMargin);
    case AP_BOTLEFT:
        return QPointF(-itemMargin, itemMargin);
    default:
        return QPointF(0,0);
    }

}



void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();
    //Set the mouse down type to the type which matches the position.
    mouseDownType = getMouseOverType(event->scenePos());
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    switch(event->button()){
    case Qt::LeftButton:
        switch(viewState){
        case NodeView::VS_NONE:
            //Goto VS_Selected
        case NodeView::VS_SELECTED:
            //Enter Selected Mode.
            if (mouseDownType != MO_NONE) {
                getNodeView()->setStateSelected();
                handleSelection(true, controlPressed);
                //Store the previous position.
                previousScenePosition = event->scenePos();

                if(mouseDownType == MO_CONNECT){
                    getNodeView()->setStateConnect();
                }
            }
            break;
        }
        break;
    }
}


void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();


    //Only if left button is down.
    if(event->buttons() & Qt::LeftButton){
        QPointF deltaPos = (event->scenePos() - previousScenePosition);

        switch (viewState){
        case NodeView::VS_SELECTED:
            if(mouseDownType == MO_RESIZE || mouseDownType == MO_RESIZE_HOR || mouseDownType == MO_RESIZE_VER){
                if(isResizeable()){
                    //Is resizing
                    getNodeView()->setStateResizing();
                }
            }else if(mouseDownType <= MO_TOPBAR && mouseDownType >= MO_ICON){
                if(isMoveable()){
                    //Moving not resizing.
                    getNodeView()->setStateMoving();
                }
            }
            break;
        case NodeView::VS_RESIZING:
            //If we are resizing horizontally, remove the vertical change.
            if(mouseDownType == MO_RESIZE_HOR){
                deltaPos.setY(0);
                //If we are resizing vertically, remove the horizontal change.
            }else if(mouseDownType == MO_RESIZE_VER){
                deltaPos.setX(0);
            }

            emit NodeItem_ResizeSelection(getID(), QSizeF(deltaPos.x(), deltaPos.y()));
            previousScenePosition = event->scenePos();
            break;
        case NodeView::VS_MOVING:
            emit NodeItem_MoveSelection(deltaPos);
            previousScenePosition = event->scenePos();
            break;
        }
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);



    //Only if left button is down.
    switch(event->button()){
    case Qt::LeftButton:{
        switch (viewState){
        case NodeView::VS_MOVING:
            emit NodeItem_MoveFinished();
            break;
        case NodeView::VS_RESIZING:
            emit NodeItem_ResizeFinished(getID());
            break;
        case NodeView::VS_SELECTED:
            if(mouseDownType == MO_HARDWAREMENU){
                emit NodeItem_ShowHardwareMenu(this);
            }
            break;
        default:
            break;
        }
        break;
    }
    case Qt::RightButton:{
        switch (viewState){
        case NodeView::VS_PAN:
            //IGNORE
        case NodeView::VS_PANNING:
            //IGNORE
            break;
        default:
            //If we haven't Panned, we need to make sure this NodeItem is selected before the toolbar opens.1
            handleSelection(true, controlPressed);
            break;
        }
        break;
    }
    case Qt::MiddleButton:{
        switch (viewState){
        case NodeView::VS_NONE:
        case NodeView::VS_SELECTED:
            if(controlPressed){
                if(!isInSubView){
                    emit GraphMLItem_TriggerAction("Sorting Node");
                    sort();
                }
            }else{
                emit GraphMLItem_SetCentered(this);
            }

            //IGNORE
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    mouseDownType = MO_NONE;
}



void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(isInSubView){
        return;
    }

    MOUSEOVER_TYPE mouseDblClickType = getMouseOverType(event->scenePos());

    int aspectID = -1;

    switch(event->button()){
    case Qt::LeftButton:{
        switch(mouseDblClickType){
        case MO_ICON:
            if(hasVisibleChildren()){
                if(isExpanded()){
                    GraphMLItem_TriggerAction("Contracted Node Item");
                }else{
                    GraphMLItem_TriggerAction("Expanded Node Item");
                }
                setNodeExpanded(!isExpanded());
                updateModelSize();
            }
            break;
        case MO_BOT_LABEL:

            break;
        case MO_RESIZE_HOR:
            //Continue
        case MO_RESIZE_VER:
            //Continue
        case MO_RESIZE:
            if(hasVisibleChildren()){
                GraphMLItem_TriggerAction("Optimizes Size of NodeItem");
                resizeToOptimumSize(mouseDblClickType);
                updateModelSize();
            }
            break;

        case MO_ITEM:
            if(isModel()){
                //If we double click on anything in the model which isn't the circle, Center Aspects.
                emit GraphMLItem_CenterAspects();
            }
            break;

        case MO_MODEL_TL:
            emit NodeItem_Model_AspectToggled(0);
            break;
        case MO_MODEL_TR:
            emit NodeItem_Model_AspectToggled(1);
            break;
        case MO_MODEL_BR:
            emit NodeItem_Model_AspectToggled(2);
            break;
        case MO_MODEL_BL:
            emit NodeItem_Model_AspectToggled(3);
            break;
        default:
            break;

        }

        break;
    }
    default:
        break;
    }
}






void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    handleHighlight(true);
}

void NodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QString tooltip;
    QCursor cursor;
    MOUSEOVER_TYPE hoverType = getMouseOverType(event->scenePos());


    switch(hoverType){

    case MO_CONNECT:
        cursor = Qt::CrossCursor;
        tooltip = "Click and drag to connect entity.";
        break;
    case MO_ICON:
        if(hasVisibleChildren()){
            cursor = Qt::PointingHandCursor;
            if(isExpanded()){
                tooltip = "Double click to contract entity.";
            }else{
                tooltip = "Double click to expand entity.";
            }
        }else if(isSelected()){
            tooltip = "Entity contains no visible children.";
        }
        break;
    case MO_EXPANDLABEL:
    case MO_TOP_LABEL:
        if(isDataEditable("label")){
            cursor = Qt::IBeamCursor;
            tooltip = tooltip  = topLabelInputItem->getValue() + "\nDouble click to edit label.";
        }else{
            tooltip  = topLabelInputItem->getValue();
        }
        break;
    case MO_BOT_LABEL:
        if(isDataEditable(editableDataKey)){
            cursor = Qt::IBeamCursor;
            tooltip = tooltip  = bottomInputItem->getValue() + "\nDouble click to edit "+ editableDataKey +".";
        }else{
            tooltip  = bottomInputItem->getValue();
        }
        break;
    case  MO_DEFINITION:
        cursor = Qt::WhatsThisCursor;
        if(HAS_DEFINITION){
            tooltip = "This entity has a definition.";
        }else if(nodeMemberIsKey){
            tooltip = "This Member is the key.";
        }
        break;
    case MO_HARDWAREMENU:
        cursor = Qt::PointingHandCursor;
        tooltip = "Showing: ";
        if(CHILDREN_VIEW_MODE == ALL){
            tooltip += "All";
        }else if(CHILDREN_VIEW_MODE == CONNECTED){
            tooltip += "Connected";
        }else if(CHILDREN_VIEW_MODE == UNCONNECTED){
            tooltip += "Unconnected";
        }
        tooltip +=" Nodes.\nClick to change the displayed Hardware Nodes.";
        break;
    case MO_DEPLOYMENTWARNING:
        cursor = Qt::WhatsThisCursor;
        tooltip = "Not all children entities are deployed to the same Hardware entity.";
        break;
    case MO_RESIZE:
        if(isSelected()){
            tooltip = "Click and drag to change size.\nDouble click to auto set size.";
            cursor = Qt::SizeFDiagCursor;
        }
        break;
    case MO_RESIZE_HOR:
        if(isSelected()){
            tooltip = "Click and drag to change width.\nDouble click to auto set width.";
            cursor = Qt::SizeHorCursor;
        }
        break;
    case MO_RESIZE_VER:
        if(isSelected()){
            tooltip = "Click and drag to change height.\nDouble click to auto set height.";
            cursor = Qt::SizeVerCursor;
        }
        break;
    }


    setToolTip(tooltip);

    if(cursor.shape() != Qt::ArrowCursor){
        setCursor(cursor);
    }else{
        unsetCursor();
    }

    handleHighlight(true);
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    handleHighlight(false);

    //Unset the cursor
    unsetCursor();
}

void NodeItem::updateTextVisibility()
{
    bool showRightLabel = false;
    bool showTopLabel = true;
    bool showBottomLabel = !editableDataKey.isEmpty();

    if(isExpanded()){
        if(!childItems().isEmpty()){
            showRightLabel = true;
        }
    }

    if(width > ACTUAL_ITEM_SIZE + GRID_PADDING_SIZE){
        showTopLabel = false;
    }

    if (topLabelInputItem && rightLabelInputItem) {
        topLabelInputItem->setVisible(showTopLabel && getRenderState() >= RS_REDUCED);
        rightLabelInputItem->setVisible(showRightLabel && getRenderState() >= RS_MINIMAL);

        bottomInputItem->setVisible(showBottomLabel && getRenderState() >= RS_FULL);
    }
}

QRectF NodeItem::adjustRectForBorder(QRectF rect, qreal borderWidth)
{
    rect.setWidth(rect.width() - borderWidth);
    rect.setHeight(rect.height() - borderWidth);
    rect.translate(borderWidth / 2, borderWidth / 2);
    return rect;
}


/**
 * @brief NodeItem::updateDisplayedChildren
 * @param viewMode
 */
void NodeItem::updateDisplayedChildren(int viewMode)
{
    // if this item is not a HardwareCLuster, do nothing
    if (!IS_HARDWARE_CLUSTER) {
        return;
    }

    // if any of the menu items are not constructed, do nothing
    if (!allChildren || !connectedChildren || !unConnectedChildren) {
        return;
    }

    // if the new view mode is the same as the current one, do nothing
    if (viewMode == CHILDREN_VIEW_MODE) {
        return;
    }

    // if viewMode = -1, it means that an edgeConstructed signal was sent
    // or a HardwareCluster is expanded and therefore need to check viewMode
    // update children HardwareNodes' visibily based on the current view mode
    if (viewMode == -1) {
        viewMode = CHILDREN_VIEW_MODE;
    }

    QList<NodeItem*> childrenItems = getChildNodeItems();
    allChildren->setChecked(false);
    connectedChildren->setChecked(false);
    unConnectedChildren->setChecked(false);

    if (viewMode == ALL) {
        // show all HarwareNodes
        allChildren->setChecked(true);
        foreach (NodeItem* item, childrenItems) {
            item->setHidden(!isNodeExpanded);
        }
    } else if (viewMode == CONNECTED) {
        // show connected HarwareNodes
        connectedChildren->setChecked(true);
        foreach (NodeItem* item, childrenItems) {
            if (item->getEdgeItemCount() > 0) {
                item->setHidden(!isNodeExpanded);
            } else {
                item->setHidden(true);
            }
        }
    } else if (viewMode == UNCONNECTED) {
        // show unconnected HarwareNodes
        unConnectedChildren->setChecked(true);
        foreach (NodeItem* item, childrenItems) {
            if (item->getEdgeItemCount() == 0) {
                item->setHidden(!isNodeExpanded);
            } else {
                item->setHidden(true);
            }
        }
    } else {
        return;
    }

    CHILDREN_VIEW_MODE = viewMode;
    sortTriggerAction = false;
    sort();
    sortTriggerAction = true;
}

QRectF NodeItem::smallIconRect() const
{
    qreal iconSize = SMALL_ICON_RATIO * minimumWidth;
    return QRectF(0, 0, iconSize, iconSize);
}


/**
 * @brief NodeItem::iconRect
 * @return - The QRectF which represents the position for the Icon
 */
QRectF NodeItem::iconRect() const
{
    qreal iconSize = ICON_RATIO * minimumWidth;
    if(getRenderState() <= RS_REDUCED){
        iconSize = minimumWidth;
    }

    //Construct a Rectangle to represent the icon size at the origin
    QRectF icon = QRectF(0, 0, iconSize, iconSize);


    QPointF centerPoint = minimumBoundingRect().center();

    icon.moveCenter(centerPoint);
    return icon;
}

QRectF NodeItem::textRect_Top() const
{
    float itemMargin = getItemMargin() * 2;
    return QRectF(QPointF(0, 0), QPointF(width + itemMargin, height + itemMargin));
}

QRectF NodeItem::textRect_Right() const
{
    QRectF icon = iconRect();

    qreal textHeight = icon.height() / 3;
    qreal textWidth = boundingRect().right() - icon.right() - getItemMargin();
    return QRectF(icon.right(), icon.center().y() - textHeight/2, textWidth, textHeight);
}

/**
 * @brief NodeItem::lockIconRect
 * @return The QRectF which represents the position for the Lock Icon
 */
QRectF NodeItem::iconRect_TopLeft() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    qreal itemMargin = getItemMargin();
    iconRect.moveTopLeft(QPointF(itemMargin, itemMargin));

    return iconRect;
}

QRectF NodeItem::iconRect_TopRight() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    qreal itemMargin = (getItemMargin());
    iconRect.moveTopRight(QPointF(itemMargin + width, itemMargin));
    return iconRect;
}

QRectF NodeItem::iconRect_BottomLeft() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    qreal itemMargin = (getItemMargin());
    iconRect.moveBottomLeft(QPointF(itemMargin, itemMargin + minimumHeight));
    return iconRect;
}

QRectF NodeItem::iconRect_BottomRight() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    qreal iconSize = MARGIN_RATIO * minimumWidth;
    QRectF iconRect =  QRectF(0, 0, iconSize, iconSize);

    //Translate to move the icon to its position
    qreal itemMargin = (getItemMargin());
    iconRect.moveBottomRight(QPointF(itemMargin + width, itemMargin + height));
    return iconRect;
}

QRectF NodeItem::getImageRect(NodeItem::IMAGE_POS pos) const
{
    switch(pos){
        case IP_BOTLEFT:
            return iconRect_BottomLeft();
        case IP_TOPRIGHT:
            return iconRect_TopRight();
        case IP_TOPLEFT:
            return iconRect_TopLeft();
        case IP_BOT_RIGHT:
            return iconRect_BottomRight();
        case IP_CENTER:
            return iconRect();
        default:
            return QRectF();
    }
}

void NodeItem::setImage(NodeItem::IMAGE_POS pos, QPixmap image)
{
    imageMap[pos] = image;
}



bool NodeItem::isInResizeMode()
{
    return currentResizeMode != NO_RESIZE;
}

bool NodeItem::isMoveable()
{
    return !(IS_DEFINITION || isModel());
}

bool NodeItem::isResizeable()
{
    return isSelected() && isExpanded() && (IS_DEFINITION || gotVisibleChildren);
}


void NodeItem::updateModelData()
{
    //Give the current Width and height. update the width/height variable in the GraphML Model.
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* wData = modelEntity->getData("width");
        GraphMLData* hData = modelEntity->getData("height");
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        wData->setValue(QString::number(width));
        hData->setValue(QString::number(height));
        QPointF center = centerPos();
        xData->setValue(QString::number(center.x()));
        yData->setValue(QString::number(center.y()));
    }
}

void NodeItem::resizeToOptimumSize(NodeItem::MOUSEOVER_TYPE type)
{
    QRectF rect = childrenBoundingRect();
    switch(type){
    case MO_RESIZE:
        setWidth(rect.width());
        setHeight(rect.height());
        break;
    case MO_RESIZE_HOR:
        setWidth(rect.width());
        break;
    case MO_RESIZE_VER:
        setHeight(rect.height());
        break;
    default:
        break;
    }
}


NodeItem *NodeItem::getChildNodeItemFromNode(Node *child)
{
    foreach(NodeItem* childNI , childNodeItems){
        if(childNI->getNode() == child){
            return childNI;
        }
    }
    return 0;
}




void NodeItem::setWidth(qreal w)
{
    if(isModel() && width != 0){
        //Can't change model after initial set.
        return;
    }

    if(isExpanded()){
        //If is expanded, we can't set height smaller than the right of the child rect.
        qreal minWidth = childrenBoundingRect().right();
        w = qMax(w, minWidth);
        expandedWidth = w;
    }else{
        //We can't shrink or grow the the minimum height.
        w = minimumWidth;
    }

    if(w == width){
        return;
    }

    prepareGeometryChange();

    width = w;


    updateTextLabel();
    calculateGridlines();
    isOverGrid(centerPos());


    if(isAspect()){
        //Force reset of position for aspects.
        setPos(0,0);
    }

    emit NodeItem_Moved();
}


void NodeItem::setHeight(qreal h)
{
    if(isModel() && height != 0){
        //Can't change model after initial set.
        return;
    }

    if(isExpanded()){
        //If is expanded, we can't set height smaller than the bottom of the child rect.
        qreal minHeight = childrenBoundingRect().bottom();

        h = qMax(h, minHeight);
        expandedHeight = h;
    }else{
        //We can't shrink or grow the the minimum height.
        h = minimumHeight;
    }

    if(h == height){
        return;
    }

    prepareGeometryChange();
    height = h;

    calculateGridlines();
    isOverGrid(centerPos());


    if(isAspect()){
        //Force reset of position for aspects.
        setPos(0,0);
    }
    emit NodeItem_Moved();
}

void NodeItem::setSize(qreal w, qreal h)
{
    setWidth(w);
    setHeight(h);
}


/**
 * @brief NodeItem::calculateGridlines Calculates the horizontal and vertical gridlines to be drawn by paint.
 */
void NodeItem::calculateGridlines()
{
    if(GRIDLINES_ON){
        QRectF boundingGridRect = gridRect();
        xGridLines.clear();
        yGridLines.clear();

        //Don't draw first.
        for(qreal x = boundingGridRect.left() + getGridSize(); x <= boundingGridRect.right(); x += getGridSize()){
            xGridLines << QLineF(x, boundingGridRect.top(), x, boundingGridRect.bottom());
        }

        for(qreal y = boundingGridRect.top() + getGridSize(); y <= boundingGridRect.bottom(); y += getGridSize()){
            yGridLines << QLineF(boundingGridRect.left(), y, boundingGridRect.right(), y);
        }
    }
}




bool NodeItem::isAncestorSelected()
{
    if(parentNodeItem){
        if(parentNodeItem->isSelected()){
            return true;
        }else{
            return parentNodeItem->isAncestorSelected();
        }
    }
    return false;
}


/**
 * @brief NodeItem::setGridVisible Sets the grid as visible.
 * @param visible
 */
void NodeItem::setGridVisible(bool visible)
{
    if(isGridVisible != visible){
        isGridVisible = visible;
        update();
    }
}


void NodeItem::updateTextLabel(QString newLabel)
{
    // apparently we do need this >_<
    if (!(topLabelInputItem || rightLabelInputItem || bottomInputItem)) {
        return;
    }

    if (newLabel != "") {
        nodeLabel = newLabel;
        topLabelInputItem->setValue(newLabel);
        rightLabelInputItem->setValue(newLabel);
    }

    //Update font size
    if(!isModel()){
        //
        qreal topWidth = boundingRect().width();
        qreal bottomWidth = iconRect_BottomRight().left() - iconRect_BottomLeft().right();
        qreal rightWidth = iconRect_BottomRight().left() - iconRect().right();


        topLabelInputItem->setWidth(topWidth);
        bottomInputItem->setWidth(bottomWidth);
        //rightLabelInputItem->setWidth(rightWidth);

        rightLabelInputItem->updatePosSize(textRect_Right());

        topLabelInputItem->setHeight(iconRect_BottomLeft().height());
        bottomInputItem->setHeight(iconRect_BottomLeft().height());
        //rightLabelInputItem->setHeight(iconRect().height() / 3);
    }

    //Calculate position for label
    qreal labelX = (boundingRect().width() - topLabelInputItem->boundingRect().width()) /2;
    qreal labelY = getItemMargin() + (ICON_RATIO * minimumHeight);



    if(isModel()){
        //The Model is centralized, so center label.
        labelX = - topLabelInputItem->boundingRect().width() / 2;
        labelY = - topLabelInputItem->boundingRect().height() / 2;
    }

    //Update position

    //Contained
    QPointF contractedLabel = QPointF(0, -topLabelInputItem->boundingRect().height());
    topLabelInputItem->setPos(contractedLabel);

    topLabelInputItem->update();
    updateTextVisibility();
}



void NodeItem::setupAspect()
{
    Node* node = getNode();

    while(node){
        QString nodeKind = node->getDataValue("kind");
        if(nodeKind == "ManagementComponent"){
            viewAspects.append("Hardware");
            viewAspects.append("Assembly");
        }else if(nodeKind == "HardwareDefinitions"){
            viewAspects.append("Hardware");
        }else if(nodeKind == "AssemblyDefinitions"){
            viewAspects.append("Assembly");
        }else if(nodeKind == "BehaviourDefinitions"){
            viewAspects.append("Workload");
        }else if(nodeKind == "InterfaceDefinitions"){
            viewAspects.append("Definitions");
        }

        if (nodeKind == "DeploymentDefintions") {
            if (!viewAspects.contains("Hardware")) {
                viewAspects.append("Hardware");
            }
            if (!viewAspects.contains("Assembly")) {
                viewAspects.append("Assembly");
            }
        }

        node = node->getParentNode();
    }
}


void NodeItem::setupBrushes()
{
    if(nodeKind == "BehaviourDefinitions"){
        color = QColor(254,184,126);
    }
    else if(nodeKind == "InterfaceDefinitions"){
        color = QColor(110,210,210);
    }
    else if(nodeKind == "HardwareDefinitions"){
        color = QColor(110,170,220);
    }
    else if(nodeKind == "AssemblyDefinitions"){
        color = QColor(255,160,160);
    }else{
        if(!parentNodeItem || (parentNodeItem && parentNodeItem->getNodeKind().endsWith("Definitions"))){
            color = QColor(233,234,237);
        }else{
            if(parentNodeItem){
                color = parentNodeItem->getBackgroundColor().darker(110);

            }
        }

    }

    selectedColor = color;
    modelCircleColor = Qt::gray;

    headerBrush = QBrush(color);

    if (nodeKind.endsWith("Definitions")) {
        bodyBrush = QBrush(color);
    } else {
        bodyBrush = QBrush(color.lighter(105));
    }

    pen.setWidth(1);
    pen.setColor(Qt::black);
    //pen.setColor(Qt::darkGray);
    selectedPen.setColor(Qt::blue);
    selectedPen.setWidth(1);
}


void NodeItem::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}


void NodeItem::setPos(const QPointF &pos)
{
    QPointF newPos = pos;


    if(IS_DEFINITION){
        //Calculate position of the aspect. It should be fixed.
        QRectF rectangle = boundingRect();
        switch(aspectPos){
        case AP_TOPLEFT:
            rectangle.moveBottomRight(aspectLockPos);
            break;
        case AP_TOPRIGHT:
            rectangle.moveBottomLeft(aspectLockPos);
            break;
        case AP_BOTRIGHT:
            rectangle.moveTopLeft(aspectLockPos);
            break;
        case AP_BOTLEFT:
            rectangle.moveTopRight(aspectLockPos);
            break;
        default:
            break;
        }
        //The top left of the rectangle represents the aspects position.
        newPos = rectangle.topLeft();
    }


    //is the new position different.
    if(newPos != this->pos()){
        prepareGeometryChange();
        QGraphicsItem::setPos(newPos);


        if(!(isAspect() || isModel())){
            //Need to check if we are over the grid.
            isOverGrid(centerPos());
        }


        emit NodeItem_Moved();

        if(getParentNodeItem()){
            getParentNodeItem()->childMoved();
        }
    }
}



/**
 * @brief NodeItem::setupChildrenViewOptionMenu
 */
void NodeItem::setupChildrenViewOptionMenu()
{
    childrenViewOptionMenu = new QMenu();

    QFont font = childrenViewOptionMenu->font();
    font.setPointSize(9);

    childrenViewOptionMenu->setFont(font);
    //childrenViewOptionMenu->setAttribute(Qt::WA_TranslucentBackground);
    //childrenViewOptionMenu->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);

    allChildren = new QRadioButton("All");
    connectedChildren = new QRadioButton("Connected");
    unConnectedChildren = new QRadioButton("Unconnected");

    QWidgetAction* a1 = new QWidgetAction(this);
    QWidgetAction* a2 = new QWidgetAction(this);
    QWidgetAction* a3 = new QWidgetAction(this);

    a1->setDefaultWidget(allChildren);
    a2->setDefaultWidget(connectedChildren);
    a3->setDefaultWidget(unConnectedChildren);

    childrenViewOptionMenu->addAction(a1);
    childrenViewOptionMenu->addAction(a2);
    childrenViewOptionMenu->addAction(a3);

    connect(allChildren, SIGNAL(clicked()), this, SLOT(hardwareClusterMenuItemPressed()));
    connect(connectedChildren, SIGNAL(clicked()), this, SLOT(hardwareClusterMenuItemPressed()));
    connect(unConnectedChildren, SIGNAL(clicked()), this, SLOT(hardwareClusterMenuItemPressed()));
    connect(allChildren, SIGNAL(clicked()), childrenViewOptionMenu, SLOT(hide()));
    connect(connectedChildren, SIGNAL(clicked()), childrenViewOptionMenu, SLOT(hide()));
    connect(unConnectedChildren, SIGNAL(clicked()), childrenViewOptionMenu, SLOT(hide()));
    connect(childrenViewOptionMenu, SIGNAL(aboutToHide()), this, SLOT(menuClosed()));

    // set the intial mode to only show connected HarwareNodes
    //connectedChildren->setChecked(true);
}



//Dont be N-Factorial
void NodeItem::childUpdated()
{
    if(!getGraphML()){
        return;
    }
    if(isModel()){
        //Sort after any model changes!
        sort();
        return;
    }

    QRectF childRect = childrenBoundingRect();

    bool okay = false;
    double modelWidth = getGraphMLDataValue("width").toDouble(&okay);
    if(!okay){
        return;
    }
    double modelHeight = getGraphMLDataValue("height").toDouble(&okay);
    if(!okay){
        return;
    }


    //Maximize on the current size in the Model and the minimum child rectangle
    if(childRect.right() > modelWidth){
        setWidth(childRect.right());
    }

    if(childRect.bottom() > modelHeight){
        setHeight(childRect.bottom());
    }


}

void NodeItem::connectToGraphMLData(GraphMLData *data)
{
    if(data){
        int ID = data->getID();
        if(!connectedDataIDs.contains(ID)){
            connect(data, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
            connectedDataIDs.append(ID);
        }
    }
}

void NodeItem::connectToGraphMLData(QString keyName)
{
    if(getGraphML()){
        connectToGraphMLData(getGraphML()->getData(keyName));
    }
}


void NodeItem::aspectsChanged(QStringList visibleAspects)
{

    bool visible = true;
    foreach(QString requiredAspect, viewAspects){
        if(!visibleAspects.contains(requiredAspect)){
            visible = false;
        }
    }

    isNodeInAspect = visible;

    // still need to update the isInAspect state in both these cases
    // just don't change the visibility of the node item
    if(hidden || isModel()){
        return;
    }

    if(getParentNodeItem() && !getParentNodeItem()->isExpanded()){
        return;
    }

    if (isNodeInAspect) {
        if (IS_HARDWARE_CLUSTER) {
            // when displaying hardware clusters, make sure that the correct children view mode is set
            updateDisplayedChildren(CHILDREN_VIEW_MODE);
        } else if (nodeKind == "HardwareNode"){
            if (parentNodeItem && parentNodeItem->getNodeKind() == "HardwareCluster") {
                // only show the HardwareNode if it matches its parent cluster's view mode
                int viewMode = parentNodeItem->getChildrenViewMode();
                if (viewMode == CONNECTED && getEdgeItemCount() == 0) {
                    visible = false;
                }
            }
        }
    }

    //bool prevVisible = isVisible();

    setVisibility(visible);


    // if not visible, unselect node item
    if (!isVisible()) {
        setSelected(false);
    }
}



/**
 * @brief NodeItem::setupLabel
 * This sets up the font and size of the label.
 */
void NodeItem::setupLabel()
{
    // this updates this item's label
    if (IS_DEFINITION){
        return;
    }

    qreal topFontSize = TOP_LABEL_RATIO * minimumHeight;
    qreal rightFontSize = RIGHT_LABEL_RATIO * minimumHeight;
    qreal botFontSize = BOTTOM_LABEL_RATIO * minimumHeight;

    bottomInputItem = new InputItem(this, "", false);
    topLabelInputItem = new InputItem(this,"", false);
    rightLabelInputItem = new InputItem(this, "", false);

    //Setup external Label
    topLabelInputItem->setAcceptHoverEvents(true);
    topLabelInputItem->setToolTipString("Double click to edit label.");
    topLabelInputItem->setCursor(Qt::IBeamCursor);


    topLabelInputItem->setAlignment(Qt::AlignCenter);
    rightLabelInputItem->setAlignment(Qt::AlignLeft  | Qt::AlignVCenter);
    bottomInputItem->setAlignment(Qt::AlignLeft  | Qt::AlignVCenter);

    //Hide it.
    bottomInputItem->setVisible(false);

    QFont textFont;
    textFont.setPixelSize(rightFontSize);
    rightLabelInputItem->setFont(textFont);
    textFont.setPixelSize(topFontSize);
    topLabelInputItem->setFont(textFont);
    textFont.setPixelSize(botFontSize);
    //textFont.setItalic(true);
    bottomInputItem->setFont(textFont);


    connect(rightLabelInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(labelEditModeRequest()));
    connect(topLabelInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(labelEditModeRequest()));
    connect(bottomInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(labelEditModeRequest()));

    connect(topLabelInputItem, SIGNAL(InputItem_ValueChanged(QString)),this, SLOT(dataChanged(QString)));
    connect(rightLabelInputItem, SIGNAL(InputItem_ValueChanged(QString)),this, SLOT(dataChanged(QString)));
    connect(bottomInputItem, SIGNAL(InputItem_ValueChanged(QString)),this, SLOT(dataChanged(QString)));


    connect(topLabelInputItem, SIGNAL(InputItem_HasFocus(bool)), this, SIGNAL(Nodeitem_HasFocus(bool)));
    connect(rightLabelInputItem, SIGNAL(InputItem_HasFocus(bool)), this, SIGNAL(Nodeitem_HasFocus(bool)));
    connect(bottomInputItem, SIGNAL(InputItem_HasFocus(bool)), this, SIGNAL(Nodeitem_HasFocus(bool)));

    QPointF bottomLabelPos = iconRect_BottomLeft().topRight();
    QPointF expandedLabelPos = expandedLabelRect().topLeft() - QPointF(0, rightLabelInputItem->boundingRect().height() /2);

    rightLabelInputItem->setPos(expandedLabelPos);
    bottomInputItem->setPos(bottomLabelPos);
    topLabelInputItem->setPos(bottomLabelPos - QPointF(0 , bottomInputItem->boundingRect().height()));


}




/**
 * @brief NodeItem::setupGraphMLConnections
 */
void NodeItem::setupGraphMLDataConnections()
{
    connectToGraphMLData("x");
    connectToGraphMLData("y");
    connectToGraphMLData("height");
    connectToGraphMLData("width");
    connectToGraphMLData("label");
    connectToGraphMLData("type");

    if(nodeKind == "HardwareNode"){
        connectToGraphMLData("os");
        connectToGraphMLData("architecture");
        connectToGraphMLData("localhost");
    }else if(nodeKind == "Member"){
        connectToGraphMLData("key");
    }
}

void NodeItem::updateGraphMLData()
{
    for(int i = 0; i < connectedDataIDs.size(); i++){
        int ID = connectedDataIDs.at(i);
        GraphMLData* data = getGraphML()->getData(ID);
        if(data){
            graphMLDataChanged(data);
        }
    }
}

QPointF NodeItem::isOverGrid(const QPointF centerPosition)
{

    if(!GRIDLINES_ON || !parentNodeItem || IS_DEFINITION){
        if(parentNodeItem){
            isNodeOnGrid = false;
            parentNodeItem->removeChildOutline(getID());
        }
        return QPointF();
    }

    QPointF gridPoint = parentNodeItem->getClosestGridPoint(centerPosition);

    //Calculate the distance between the centerPosition and the closestGrid
    qreal distance = QLineF(centerPosition, gridPoint).length();

    bool isMoving = getNodeView()->getViewState() == NodeView::VS_MOVING || getNodeView()->getViewState() == NodeView::VS_RESIZING;
    //If the distance is less than the SNAP_PERCENTAGE
    if((distance / minimumWidth) <= SNAP_PERCENTAGE){
        if(isNodeOnGrid || isMoving){
            isNodeOnGrid = true;
            parentNodeItem->addChildOutline(this, gridPoint);
        }
        return gridPoint;
    }else{
        isNodeOnGrid = false;

        if(isMoving){
            parentNodeItem->removeChildOutline(getID());
        }
        return QPointF();
    }

}




void NodeItem::toggleGridLines(bool on)
{
    if(on != GRIDLINES_ON){
        GRIDLINES_ON = on;
        if(on){
            calculateGridlines();
        }
    }
}




void NodeItem::snapToGrid()
{
    if(parentNodeItem && isVisible()){
        QPointF gridPosition = parentNodeItem->getClosestGridPoint(centerPos());

        setCenterPos(gridPosition);
    }
}


void NodeItem::snapChildrenToGrid()
{
    foreach(NodeItem *child, childNodeItems){
        if(child->isVisible()){
            child->snapToGrid();
            /*
            QPointF localPosition = child->minimumVisibleRect().center();
            QPointF childPosition = child->pos() + localPosition;
            QPointF newPosition = getClosestGridPoint(childPosition);

            if(childPosition != newPosition){
                child->setPos(newPosition - localPosition);
                // this needs to be called because setPos isn't immediately updating the node's pos
                // if it's not called, the item jumps back to it's prev pos when dragged
                //child->updateGraphMLPosition();
            }
            */
        }
    }
}


/**
 * @brief NodeItem::menuClosed
 */
void NodeItem::menuClosed()
{
    emit NodeItem_lockMenuClosed(this);
}


/**
 * @brief NodeItem::updateChildrenViewMode
 * @param viewMode
 */
void NodeItem::updateChildrenViewMode(int viewMode)
{
    updateDisplayedChildren(viewMode);
}


/**
 * @brief NodeItem::hardwareClusterMenuItemPressed
 */
void NodeItem::hardwareClusterMenuItemPressed()
{
    // if the sender was an action, it means that this was triggered from the menu
    QRadioButton* action = qobject_cast<QRadioButton*>(QObject::sender());

    if (action) {

        int value = -1;

        if (action == allChildren) {
            value = ALL;
        } else if (action == connectedChildren) {
            value = CONNECTED;
        } else if (action == unConnectedChildren) {
            value = UNCONNECTED;
        } else {
            return;
        }

        emit nodeItem_HardwareMenuClicked(value);
    }
}


/**
 * @brief NodeItem::getHardwareClusterChildrenViewMode
 * @return
 */
int NodeItem::getHardwareClusterChildrenViewMode()
{
    return CHILDREN_VIEW_MODE;
}


/**
 * @brief NodeItem::themeChanged
 * @param theme
 */
void NodeItem::themeChanged(int theme)
{
    QString bgColor = "rgba(240,240,240,250);";
    QString textColor = "black;";
    QString checkedColor = "green;";
    modelCircleColor = Qt::gray;
    topRightColor = QColor(254,184,126);
    topLeftColor = QColor(110,210,210);
    bottomRightColor = QColor(110,170,220);
    bottomLeftColor = QColor(255,160,160);

    switch (theme) {
    case THEME_DARK:
        bgColor = "rgba(130,130,130,250);";
        textColor = "white;";
        checkedColor = "yellow;";
        //modelCircleColor = QColor(100,100,100);
        modelCircleColor = QColor(70,70,70);
        if (darkThemeType == THEME_DARK_NEUTRAL) {
            topLeftColor = QColor(134,161,170);
            topRightColor = QColor(164,176,172);
            bottomLeftColor = QColor(192,191,197);
            bottomRightColor = QColor(239,238,233);
        } else if (darkThemeType == THEME_DARK_COLOURED) {
            topLeftColor = QColor(24,148,184);
            topRightColor = QColor(155,155,155);
            bottomLeftColor = QColor(90,90,90);
            bottomRightColor = QColor(207,107,100);
        }
        break;
    default:
        break;
    }

    if (isAspect()) {
        if (nodeKind == "BehaviourDefinitions") {
            bodyBrush.setColor(topRightColor);
        } else if (nodeKind == "InterfaceDefinitions") {
            bodyBrush.setColor(topLeftColor);
        } else if (nodeKind == "HardwareDefinitions") {
            bodyBrush.setColor(bottomRightColor);
        } else if (nodeKind == "AssemblyDefinitions") {
            bodyBrush.setColor(bottomLeftColor);
        }
    }

    if (IS_HARDWARE_CLUSTER && childrenViewOptionMenu) {
        childrenViewOptionMenu->setStyleSheet("QRadioButton {"
                                              "padding: 8px 10px 8px 8px;"
                                              "color:" + textColor +
                                              "}"
                                              "QRadioButton::checked {"
                                              "color:" + checkedColor +
                                              "font-weight: bold; "
                                              "}");
    }
}


void NodeItem::updateGraphMLPosition()
{
    //Give the current Width and height. update the width/height variable in the GraphML Model.
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        QPointF center = centerPos();
        xData->setValue(QString::number(center.x()));
        yData->setValue(QString::number(center.y()));
    }

}




void NodeItem::retrieveGraphMLData()
{

    if(!getGraphML()){
        return;
    }

    if(!getGraphML()->isDeleting()){
        graphMLDataChanged(getGraphML()->getData("width"));
        graphMLDataChanged(getGraphML()->getData("height"));
        graphMLDataChanged(getGraphML()->getData("x"));
        graphMLDataChanged(getGraphML()->getData("y"));
        graphMLDataChanged(getGraphML()->getData("label"));
        graphMLDataChanged(getGraphML()->getData("type"));
    }
}


/**
 * @brief NodeItem::getChildNodeItems
 * @return
 */
QList<NodeItem *> NodeItem::getChildNodeItems()
{
    QList<NodeItem*> insertOrderList;

    foreach(int ID, childrenIDs){
        insertOrderList.append(childNodeItems[ID]);
    }
    return insertOrderList;

}


qreal NodeItem::getGridSize()
{
    return GRID_SIZE;
    //return getChildBoundingRect().width() / GRID_RATIO;
}

qreal NodeItem::getGridGapSize()
{
    return GRID_PADDING_SIZE;
    /*
    int roundUp = ceil(GRID_RATIO);
    qCritical() << roundUp;
    qreal gap = roundUp - GRID_RATIO;
    return getChildBoundingRect().width() / gap;*/
}


/**
 * @brief NodeItem::getAspects
 * This returns the view aspects that this node item belongs to.
 * @return
 */
QStringList NodeItem::getAspects()
{
    return viewAspects;
}



bool NodeItem::isInAspect()
{
    return isNodeInAspect;
}


/**
 * @brief NodeItem::isInAspect
 * This returns whether this node item is in the currently viewed aspects or not.
 * @return
 */
/*
bool NodeItem::isInAspect()
{
    // if this node item doesn't belong to any view aspects,
    // or there are currently no view aspects turned on, return false
    if (viewAspects.count() == 0 || currentViewAspects.count() == 0) {
        return false;
    }

    // otherwise, check the list of currently viewed aspects to see if it contains
    // all of the aspects that need to be turned on for this item to be visible
    bool inAspect = true;

    for(int i = 0; i < viewAspects.size();i++){
        QString aspect = viewAspects[i];
        if(!currentViewAspects.contains(aspect)){
            inAspect = false;
            break;
        }
    }

    return inAspect;
}
*/






/**
 * @brief NodeItem::showHardwareIcon
 * When the deployment view is on, this shows the red hardware icon denoting
 * that this item has a child that is deployed to a different hardware node.
 * @param show
 */
void NodeItem::showHardwareIcon(bool show)
{
    showDeploymentWarningIcon = show;
    //update(deploymentIconRect());
}


/**
 * @brief NodeItem::deploymentView
 * @param on
 * @param selectedItem
 * @return
 */
QList<NodeItem*> NodeItem::deploymentView(bool on, NodeItem *selectedItem)
{
    QList<NodeItem*> chlidrenDeployedToDifferentNode;

    if (on) {

        Node* deploymentLink = 0;

        // get the hardware node that this item is deployed to
        foreach (Edge* edge, getNode()->getEdges(0)) {
            if (edge->isDeploymentLink()) {
                deploymentLink = edge->getDestination();
                break;
            }
        }

        // if this item isn't connected to a hardware node, do nothing
        if (!deploymentLink) {
            return chlidrenDeployedToDifferentNode;
        }

        // check this item's children's deployment links
        foreach (NodeItem* childItem, getChildNodeItems()) {
            foreach (Edge* edge, childItem->getNode()->getEdges(0)) {
                if (edge->isDeploymentLink() && edge->getDestination() != deploymentLink) {
                    if (selectedItem && selectedItem == this) {
                        childItem->setHardwareHighlighting(true);
                        //childItem->highlightNodeItem(true);
                    }
                    chlidrenDeployedToDifferentNode.append(childItem);
                    break;
                }
            }
        }

        // if there are children deployed to a different node, show red hardware icon
        if (chlidrenDeployedToDifferentNode.isEmpty()) {
            showHardwareIcon(false);
        } else {
            showHardwareIcon(true);
        }

    } else {

        // remove highlight and hide the red hradware icon
        foreach (NodeItem* childItem, getChildNodeItems()) {
            childItem->setHardwareHighlighting(false);
            //childItem->highlightNodeItem(false);
        }

        showHardwareIcon(false);
    }

    // need to update here otherwise the visual changes aren't applied till the mouse is moved
    update();

    return chlidrenDeployedToDifferentNode;
}


/**
 * @brief NodeItem::getChildrenViewMode
 * @return
 */
int NodeItem::getChildrenViewMode()
{
    return CHILDREN_VIEW_MODE;
}


void NodeItem::labelEditModeRequest()
{
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());
    if (inputItem) {
        QString dataKey = "label";
        bool comboBox = false;
        if (inputItem == bottomInputItem) {
            dataKey = editableDataKey;
            comboBox = editableDataDropDown;
        }
        if (isDataEditable(dataKey)) {
            if (comboBox) {
                QString currentValue = bottomInputItem->getValue();
                QPointF botLeft = inputItem->sceneBoundingRect().bottomLeft();
                QPointF botRight = inputItem->sceneBoundingRect().bottomRight();
                QLineF botLine = QLineF(botLeft,botRight);
                getNodeView()->showDropDown(this, botLine, editableDataKey, currentValue);
            } else {
                inputItem->setEditMode(true);
            }
        }
    }
}

void NodeItem::dataChanged(QString dataValue)
{
    //Determine the sender
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());

    if(inputItem && getGraphML()){

        QString keyValue = "label";

        if(inputItem == bottomInputItem){
            keyValue = editableDataKey;
        }

        if(!getGraphML()->getData(keyValue)->isProtected()){
            GraphMLItem_TriggerAction("Set New Data Value");
            GraphMLItem_SetGraphMLData(getID(), keyValue, dataValue);
        }
    }
}


void NodeItem::childMoved()
{
    if(!getGraphML() || isModel()){
        return;
    }

    QRectF childRect = childrenBoundingRect();

    //Maximize on the current size in the Model and the childBounding rectangle.
    if(childRect.right() > width){
        setWidth(childRect.right());
    }

    if(childRect.bottom() > height){
        setHeight(childRect.bottom());
    }
}

void NodeItem::zoomChanged(qreal currentZoom)
{
    qreal visibleWidth = currentZoom * minimumWidth;
    if(visibleWidth >= minimumWidth){
        setRenderState(RS_FULL);
    }else if(visibleWidth >= (minimumWidth / 2)){
        setRenderState(RS_REDUCED);
    }else{
        setRenderState(RS_MINIMAL);
    }

    updateTextVisibility();

    //update penWidth.
    selectedPenWidth = qMax(SELECTED_LINE_WIDTH / currentZoom, 1.0);

    //Call base class
    GraphMLItem::zoomChanged(currentZoom);
}



bool NodeItem::isSorted()
{
    return isNodeSorted;
}

void NodeItem::setSorted(bool isSorted)
{
    isNodeSorted = isSorted;
}


/**
 * @brief NodeItem::getChildrenViewOptionMenu
 * @return
 */
QMenu *NodeItem::getChildrenViewOptionMenu()
{
    return childrenViewOptionMenu;
}


/**
 * @brief NodeItem::geChildrenViewOptionMenuSceneRect
 * @return
 */
QRectF NodeItem::geChildrenViewOptionMenuSceneRect()
{
    if (IS_HARDWARE_CLUSTER) {
        QRectF menuButtonRect = mapRectToScene(iconRect_TopLeft());
        return menuButtonRect;
    }
    return QRectF();
}




QString NodeItem::getIconURL()
{
    QString imageURL = nodeKind;
    if(nodeKind == "HardwareNode"){
        if(nodeHardwareLocalHost){
            imageURL = "Localhost";
        }else{
            imageURL = nodeHardwareOS.remove(" ") + "_" + nodeHardwareArch;
        }
    }

    return imageURL;
}

void NodeItem::paintPixmap(QPainter *painter, NodeItem::IMAGE_POS pos, QString alias, QString imageName)
{
    QRectF place = getImageRect(pos);
    QPixmap image = imageMap[pos];

    if(image.isNull()){
        image = getNodeView()->getImage(alias, imageName);
        imageMap[pos] = image;
    }

    painter->drawPixmap(place.x(), place.y(), place.width(), place.height(), image);
}



/**
 * @brief NodeItem::getChildKind
 * This returns a list of kinds of all this item's children.
 * @return
 */
QStringList NodeItem::getChildrenKind()
{
    QStringList childrenKinds;
    Node *node = dynamic_cast<Node*>(getGraphML());
    if (node) {

        foreach(Node* child, node->getChildren(0)){
            childrenKinds.append(child->getDataValue("kind"));
        }
    }
    return childrenKinds;
}




QSizeF NodeItem::getModelSize()
{

    float graphmlHeight = 0;
    float graphmlWidth = 0;

    graphmlWidth = getGraphMLDataValue("width").toDouble();
    graphmlHeight = getGraphMLDataValue("height").toDouble();




    return QSizeF(graphmlWidth, graphmlHeight);
}

bool NodeItem::canHover()
{
    if(getNodeKind().endsWith("Definitions") || getNodeKind() == "Model"){
        return false;
    }
    return GraphMLItem::canHover();
}



bool NodeItem::drawGridlines()
{
    return isGridVisible && GRIDLINES_ON;
}


double NodeItem::getItemMargin() const
{
    return (MARGIN_RATIO * ITEM_SIZE);
}

double NodeItem::getChildItemMargin()
{
    return getItemMargin();
}

double NodeItem::ignoreInsignificantFigures(double model, double current)
{
    double absDifferences = fabs(model-current);
    if(absDifferences > .1){
        return current;
    }
    return model;
}


/**
 * @brief NodeItem::expandItem
 * @param show
 */
void NodeItem::setNodeExpanded(bool expanded)
{
    //Can't Contract a Definition or Model
    if(!canNodeBeExpanded){
        isNodeExpanded = true;
        return;
    }

    //If our state is already set, don't do anything!
    if(isNodeExpanded == expanded){
        return;
    }

    isNodeExpanded = expanded;

    // if expanded, only show the HardwareNodes that match the current chidldren view mode
    if (IS_HARDWARE_CLUSTER && expanded) {

        // this will show/hide HardwareNodes depending on the current view mode
        updateDisplayedChildren(-1);

        // this sets the width and height to their expanded values
        setWidth(expandedWidth);
        setHeight(expandedHeight);

        return;
    }

    //Show/Hide the non-hidden children.
    foreach(NodeItem* nodeItem, childNodeItems){
        if (!nodeItem->isHidden()){
            nodeItem->setVisibility(expanded);
        }
    }

    if(isExpanded()){
        //Set the width/height to their expanded values.
        setWidth(expandedWidth);
        setHeight(expandedHeight);
    } else {
        //Set the width/height to their minimum values.
        setWidth(minimumWidth);
        setHeight(minimumHeight);
    }
}






void NodeItem::updateModelPosition()
{
    //Update the Parent Model's size first to make sure that the undo states are correct.
    if(IS_DEFINITION){
        return;
    }

    bool isMoving = getNodeView()->getViewState() == NodeView::VS_MOVING || getNodeView()->getViewState() == NodeView::VS_RESIZING;

    //if we are over a grid line (or within a snap ratio)
    QPointF gridPoint = isOverGrid(centerPos());
    if(!gridPoint.isNull()){
        //Setting new Center Point
        setCenterPos(gridPoint);
        //If the node moved via the mouse, lock it.
        if(isSelected() && isMoving){
            setLocked(isNodeOnGrid);
        }
    }
    if(parentNodeItem){
        parentNodeItem->updateModelSize();
    }

    GraphMLItem_SetGraphMLData(getID(), "x", QString::number(centerPos().x()));
    GraphMLItem_SetGraphMLData(getID(), "y", QString::number(centerPos().y()));


    if(!isNodeOnGrid && parentNodeItem){
        parentNodeItem->removeChildOutline(getID());
    }


    GraphMLItem_PositionSizeChanged(this);
}

void NodeItem::updateModelSize()
{
    if(isModel()){
        //return;
    }
    if(this->parentNodeItem){
        parentNodeItem->updateModelSize();
    }
    //Update the Size in the model.
    GraphMLItem_SetGraphMLData(getID(), "width", QString::number(width));
    GraphMLItem_SetGraphMLData(getID(), "height", QString::number(height));

    bool isMoving = getNodeView()->getViewState() == NodeView::VS_MOVING || getNodeView()->getViewState() == NodeView::VS_RESIZING;

    //If we are over a gridline already.
    if(isNodeOnGrid){
        //Update the gridPoint.
        isOverGrid(centerPos());
    }


    if(isMoving){
        setLocked(isNodeOnGrid);
    }

    //if (width > prevWidth || height > prevHeight) {
    GraphMLItem_PositionSizeChanged(this, true);
    //}
}


/**
 * @brief NodeItem::updateSceneRect
 * This gets called everytime there has been a change to the view's sceneRect.
 * @param sceneRect
 */
void NodeItem::sceneRectChanged(QRectF sceneRect)
{
    currentSceneRect = sceneRect;
}

void NodeItem::labelUpdated(QString newLabel)
{
    if(getGraphML()){

        QString currentLabel = getGraphMLDataValue("label");

        if(currentLabel != newLabel){
            if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                GraphMLItem_TriggerAction("Set New Label");
                GraphMLItem_SetGraphMLData(getID(), "label", newLabel);
            }
        }
    }
}

void NodeItem::setNewLabel(QString newLabel)
{
    if(getGraphML()){
        if(newLabel != ""){
            if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                GraphMLItem_TriggerAction("Set New Label");
                GraphMLItem_SetGraphMLData(getID(), "label", newLabel);
            }
        }else{
            if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                topLabelInputItem->setEditMode(true);
            }
        }
    }

}


Node *NodeItem::getNode()
{
    return (Node*) getGraphML();
}


/**
 * @brief NodeItem::getNodeKind
 * @return
 */
QString NodeItem::getNodeKind()
{
    return nodeKind;
}

QString NodeItem::getNodeLabel()
{
    return nodeLabel;
}


/**
 * @brief NodeItem::setHidden
 * This method is used to prevent this item from being shown
 * when the view aspects are changed. If this item is meant to
 * be hidden no matter the view aspect, this keeps it hidden.
 */
void NodeItem::setHidden(bool h)
{
    hidden = h;
    //setVisible(!h);

    // when the item is no longer hidden, we needs to check if it's in aspect
    // and if its parent is visible before we can display it in the view
    bool parentExpanded = true;
    if (getParentNodeItem()) {
        parentExpanded = getParentNodeItem()->isExpanded();
        getParentNodeItem()->childHidden();
    }
    setVisibility(!h && isInAspect() && parentExpanded);
}


/**
 * @brief NodeItem::resetSize
 * Reset this node item's size to its default size.
 */
void NodeItem::resetSize()
{
    GraphMLItem_SetGraphMLData(getID(), "height", QString::number(minimumHeight));
    GraphMLItem_SetGraphMLData(getID(), "width", QString::number(minimumWidth));
}


/**
 * @brief NodeItem::isExpanded
 */
bool NodeItem::isExpanded()
{
    return isNodeExpanded;
}

bool NodeItem::isContracted()
{
    if(this->childItems().count() ==0){
        return true;
    }
    return !isExpanded();
}

/**
 * @brief NodeItem::getClosestGridPoint
 * @param centerPoint - The Center Point of a newly constructed child
 * @return The closest Grid Point which can contain a child!
 */
QPointF NodeItem::getClosestGridPoint(QPointF centerPoint)
{
    //Get the gridSize
    qreal gridSize = getGridSize();

    //Offset the centerPoint by the starting position of the gridRect so we can do easy division.
    QPointF gridOffset = gridRect().topLeft();
    centerPoint -= gridOffset;

    //Find the closest gridline to the centerPoint, then add the offset again.
    centerPoint.setX((qRound(centerPoint.x() / gridSize) * gridSize) + gridOffset.x());
    centerPoint.setY((qRound(centerPoint.y() / gridSize) * gridSize) + gridOffset.y());

    //Calculate the Bounding Rect of the the child.
    QPointF childOffset = QPointF(getChildWidth()/2, getChildHeight()/2);
    QRectF childRect = QRectF(centerPoint - childOffset, centerPoint + childOffset);

    //Check for collision with the parent boudning box.
    if(!gridRect().contains(childRect)){
        //Make sure the Top of the child is fully contained!
        while(childRect.top() <= gridRect().top()){
            childRect.translate(0, gridSize);
        }
        //Make sure the Left of the child is fully contained!
        while(childRect.left() <= gridRect().left()){
            childRect.translate(gridSize, 0);
        }
        return childRect.center();
    }

    return centerPoint;
}
