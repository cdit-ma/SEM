#include "entityitem.h"
#include "edgeitem.h"
#include "../nodeview.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QBitmap>

#include "../../Controller/behaviournodeadapter.h"
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



/**
 * @brief EntityItem::EntityItem
 * @param node
 * @param parent
 * @param aspects
 * @param IN_SUBVIEW
 */
EntityItem::EntityItem(NodeAdapter *node, NodeItem *parent):  NodeItem(node, parent, GraphMLItem::ENTITY_ITEM)
{
    IS_EXPANDED_STATE = false;
    if(parent && parent->isEntityItem()){
        parentEntityItem = (EntityItem*)parent;
        setVisibility(parentEntityItem->isExpandedState());

        connect(parent, SIGNAL(GraphMLItem_PositionChanged()), this, SIGNAL(GraphMLItem_PositionChanged()));
        connect(parent, SIGNAL(GraphMLItem_SizeChanged()), this, SIGNAL(GraphMLItem_PositionChanged())); // ???

    }else{
        parentEntityItem = 0;
    }

    IS_READ_ONLY = false;
    IS_READ_ONLY_DEF = false;
    IS_READ_ONLY_SNIPPET = false;

    isInputParameter = false;
    isReturnParameter = false;
    hasEditData = false;

    nodeHardwareLocalHost = false;
    nodeMemberIsKey = false;
    IS_HARDWARE_NODE = false;

    //Setup initial states
    canNodeBeConnected = false;
    isNodeOnGrid = false;
    isNodeSorted = false;
    isGridVisible = false;

    sortTriggerAction = true;
    eventFromMenu = true;
    hasWarning = false;

    isHardwareLink = false;

    showDeploymentWarningIcon = false;
    hasHardwareWarning = false;
    GRIDLINES_ON = false;
    hidden = false;

    HAS_DEFINITION = false;
    IS_IMPL_OR_INST = false;

    editableDataKey = "type";

    if(node){
        connect(node, SIGNAL(gotDefinition(bool)), this, SLOT(gotDefinition(bool)));
        IS_IMPL_OR_INST = node->isInstance() || node->isImpl();

        if(IS_IMPL_OR_INST){
            HAS_DEFINITION = node->gotDefinition();
        }
    }

    currentResizeMode = EntityItem::NO_RESIZE;

    childrenViewOptionMenu = 0;

    nodeKind = getEntityAdapter()->getDataValue("kind").toString();
    isInputParameter = (nodeKind == "InputParameter");
    isReturnParameter = (nodeKind == "ReturnParameter");

    vectorIconURL = nodeKind;
    changeIcon = false;


    //Setup labels.
    nodeLabel = "";
    nodeType = "";

    topLabelInputItem = 0;
    rightLabelInputItem = 0;
    bottomInputItem = 0;
    childrenViewOptionMenu = 0;
    allChildren = 0;
    connectedChildren = 0;
    unConnectedChildren = 0;

    //Setup initial sizes
    //Set Minimum Size.
    contractedWidth = ITEM_SIZE;
    contractedHeight = ITEM_SIZE;

    if(isInputParameter || isReturnParameter){
        contractedWidth /=2;
        contractedHeight /=2;
    }



    //Set Maximum Size
    expandedWidth = ITEM_SIZE;
    expandedHeight = ITEM_SIZE;


    IS_HARDWARE_CLUSTER = (nodeKind == "HardwareCluster");
    IS_HARDWARE_NODE = (nodeKind == "HardwareNode");
    IS_VECTOR = nodeKind.startsWith("Vector");


    if(inMainView()){
        CHILDREN_VIEW_MODE = CONNECTED;
    }else{
        CHILDREN_VIEW_MODE = ALL;
    }

    childrenViewOptionMenu = 0;
    allChildren = 0;
    connectedChildren = 0;
    unConnectedChildren = 0;

    setupLabel();

    setupDataConnections();
    updateFromData();

    setupBrushes();

    setupChildrenViewOptionMenu();
    if (IS_HARDWARE_CLUSTER) {
        if (getNodeView()) {
            themeChanged(getNodeView()->getTheme());
        } else {
            themeChanged(VT_NORMAL_THEME);
        }
    }

    updateTextLabel();
    updateErrorState();

    //Force a zoom change.
    zoomChanged(getZoomFactor());
}


/**
 * @brief EntityItem::~EntityItem
 */
EntityItem::~EntityItem()
{
    currentLeftEdgeIDs.clear();
    currentRightEdgeIDs.clear();
    delete rightLabelInputItem;
    delete topLabelInputItem;
    delete bottomInputItem;
    delete statusItem;
}

/**
 * @brief EntityItem::getMouseOverType Returns the type of the node the position
 * @param scenePos - The scene coordinate to check.
 * @return - The type the
 */
EntityItem::MOUSEOVER_TYPE EntityItem::getMouseOverType(QPointF scenePos)
{
    QPointF itemPos = mapFromScene(scenePos);

    //Most mouse over states are only useful when things are actually rendered!
    RENDER_STATE state = getRenderState();


    if(contains(itemPos)){
        if(mouseOverBotInput(itemPos) && state > RS_REDUCED){
            return MO_BOT_LABEL;
        }
        if(mouseOverBotInputIcon(itemPos) && state > RS_REDUCED){
            return MO_BOT_LABEL_ICON;
        }
        if(mouseOverRightLabel(itemPos) && state >= RS_REDUCED){
            return MO_EXPANDLABEL;
        }
        if(mouseOverConnect(itemPos) && state > RS_REDUCED){
            return MO_CONNECT;
        }

        if(mouseOverDefinition(itemPos) && state > RS_REDUCED){
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
            EntityItem::RESIZE_TYPE resize = resizeEntered(itemPos);
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
            return MO_ITEM;
        }
    }
    return MO_NONE;
}

void EntityItem::setEditableField(QString keyName, bool dropDown)
{
    editableDataKey = keyName;
    editableDataDropDown = dropDown;


    listenForData(keyName);

    if(bottomInputItem){
        bottomInputItem->setDropDown(dropDown);

        if(keyName.isEmpty()){
            bottomInputItem->setVisible(false);
        }else{
            hasEditData = true;
            bottomInputItem->setVisible(true);
        }
    }

    if(getEntityAdapter()){
        dataChanged(keyName, getEntityAdapter()->getDataValue(keyName));
    }


}

void EntityItem::setStatusField(QString keyName, bool isInt)
{
    statusModeDataKey = keyName;

    bool gotkey = false;
    if(keyName != ""){
        if(hasGraphMLKey(keyName)){
            listenForData(keyName);

            if(statusItem){
                statusItem->setNumberMode(isInt);
            }
            gotkey = true;
        }
    }
    if(!gotkey){
        statusItem->setVisible(false);
    }

    if(getEntityAdapter()){
        dataChanged(keyName, getEntityAdapter()->getDataValue(keyName));
    }
}

/**
 * @brief EntityItem::setZValue Overides the QGraphicsItem::setZValue function to recurse up it's parentEntityItem and set the Z-Value on its parents.
 * @param z The ZValue to set.
 */
void EntityItem::setZValue(qreal z)
{
    oldZValue = zValue();
    if(parentEntityItem){
        parentEntityItem->setZValue(z);
    }
    QGraphicsItem::setZValue(z);
}

/**
 * @brief EntityItem::restoreZValue Resets the Z-Value back to it's original Z-Value (Calls SetZValue())
 */
void EntityItem::restoreZValue()
{
    QGraphicsItem::setZValue(oldZValue);
    if(parentEntityItem){
        parentEntityItem->restoreZValue();
    }
}

/**
 * @brief EntityItem::setNodeConnectable Sets whether or not this node has a visible icon to allow connections to be "drawn"
 * @param connectable Is this node able to connect visually.
 */
void EntityItem::setNodeConnectable(bool connectable)
{
    if(connectable != canNodeBeConnected){
        canNodeBeConnected = connectable;
        update();
    }
}

void EntityItem::handleExpandState(bool newState)
{
    if(newState != IS_EXPANDED_STATE){
        prepareGeometryChange();
    }
    IS_EXPANDED_STATE = newState;

    //Show/Hide the non-hidden children.
    foreach(GraphMLItem* child, getChildren()){
        if(child){
            child->setVisible(IS_EXPANDED_STATE);
        }
    }


    // if expanded, only show the HardwareNodes that match the current chidldren view mode
    if (IS_HARDWARE_CLUSTER && IS_EXPANDED_STATE) {
        // this will show/hide HardwareNodes depending on the current view mode
        updateDisplayedChildren(CHILDREN_VIEW_MODE);
    }

    emit GraphMLItem_SizeChanged();

    updateTextLabel();
}


QColor EntityItem::getBackgroundColor()
{
    return headerBrush.color();
}


/**
 * @brief EntityItem::setVisibleParentForEdgeItem
 * @param line
 * @param RIGH
 * @return The index of the
 */
void EntityItem::setVisibleParentForEdgeItem(int ID, bool RIGHT)
{
    if(RIGHT){
        if(!currentRightEdgeIDs.contains(ID)){
            currentRightEdgeIDs.append(ID);
            if(currentRightEdgeIDs.size() > 1){
                //emit EntityItem_Moved();
            }
        }
    }else{
        if(!currentLeftEdgeIDs.contains(ID)){
            currentLeftEdgeIDs.append(ID);
            if(currentLeftEdgeIDs.size() > 1){
                //emit EntityItem_Moved();
            }
        }

    }
}

int EntityItem::getIndexOfEdgeItem(int ID, bool RIGHT)
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

int EntityItem::getNumberOfEdgeItems(bool RIGHT)
{
    if(RIGHT){
        return currentRightEdgeIDs.count();
    }else{
        return currentLeftEdgeIDs.count();
    }
}

void EntityItem::removeVisibleParentForEdgeItem(int ID)
{
    if(!isDeleting()){
        currentRightEdgeIDs.removeOne(ID);
        currentLeftEdgeIDs.removeOne(ID);
    }
}


EntityItem *EntityItem::getParentEntityItem()
{
    return dynamic_cast<EntityItem*>(getParent());
}



QRectF EntityItem::boundingRect() const
{
    qreal top = 0;
    qreal bot = 0;
    qreal left = 0;
    qreal right = 0;

    //Top left Justified.
    float itemMargin = getItemMargin() * 2;
    if(IS_EXPANDED_STATE){
        right += expandedWidth;
        bot += expandedHeight;
    }else{
        right += contractedWidth;
        bot += contractedHeight;
    }

    right += itemMargin;
    bot += itemMargin;
    return QRectF(QPointF(left, top), QPointF(right, bot));
}


QRectF EntityItem::minimumRect() const
{
    qreal itemMargin = getItemMargin() * 2;
    return QRectF(QPointF(0, 0), QPointF(contractedWidth + itemMargin, contractedHeight + itemMargin));
}

QRectF EntityItem::sceneBoundingRect() const
{
    QRectF newRect = GraphMLItem::sceneBoundingRect();
    newRect = newRect.united(topLabelInputItem->sceneBoundingRect());
    return newRect;
}

QRectF EntityItem::expandedBoundingRect() const
{
    qreal itemMargin = getItemMargin() * 2;
    return QRectF(QPointF(0, 0), QPointF(expandedWidth + itemMargin, expandedHeight + itemMargin));
}

QRectF EntityItem::expandedLabelRect() const
{
    qreal itemMargin = getItemMargin();

    qreal right = expandedWidth + itemMargin;
    qreal left = itemMargin + contractedWidth;
    qreal top = (itemMargin * 2 + contractedHeight) /2;

    return QRectF(QPointF(left, top), QPointF(right, top));
}


/**
 * @brief EntityItem::gridRect Returns a QRectF which contains the local coordinates of where the Grid lines are to be drawn.
 * @return The grid rectangle
 */
QRectF EntityItem::gridRect() const
{
    qreal itemMargin = getItemMargin();

    QPointF topLeft(itemMargin, itemMargin);
    QPointF bottomRight = topLeft + QPointF(getExpandedWidth(), getExpandedHeight());


    int roundedGridCount = ceil(contractedHeight / getGridSize());
    topLeft.setY(roundedGridCount * getGridSize() - getGridGapSize()/2);

    //Enforce at least one grid in height. Size!
    qreal deltaY = bottomRight.y() - topLeft.y();
    if(deltaY <= 0){
        deltaY = qMax(deltaY, getGridSize());
        bottomRight.setY(bottomRight.y() + deltaY);
    }

    return QRectF(topLeft, bottomRight);
}


QRectF EntityItem::headerRect()
{
    qreal itemMargin = 2 * getItemMargin();
    return QRectF(QPointF(0, 0), QPointF(getCurrentWidth() + itemMargin, contractedHeight + itemMargin));
}


QRectF EntityItem::bodyRect()
{
    qreal itemMargin = 2 * getItemMargin();
    return QRectF(QPointF(0, contractedHeight + itemMargin), QPointF(getWidth() + itemMargin, getHeight() + itemMargin));
}

QRectF EntityItem::getChildBoundingRect()
{
    return QRectF(QPointF(0, 0), QPointF(getChildWidth() + (2* getChildItemMargin()), getChildHeight() + (2 * getChildItemMargin())));
}


QPointF EntityItem::getGridPosition(int x, int y)
{
    QPointF topLeft = gridRect().topLeft();
    topLeft.setX(topLeft.x() + (x * getGridSize()));
    topLeft.setY(topLeft.y() + (y * getGridSize()));
    return topLeft;
}

bool EntityItem::isHardwareHighlighted()
{
    return hasHardwareWarning;
}

bool EntityItem::isNodeReadOnly()
{
    return IS_READ_ONLY;
}

bool EntityItem::isNodeReadOnlyDefinition()
{
    return IS_READ_ONLY_DEF;
}

void EntityItem::setHardwareHighlighting(bool highlighted)
{
    hasHardwareWarning = highlighted;
    update();
}




/**
 * @brief EntityItem::isHardwareCluster
 * Return if this EntityItem is a HarwareCluster or not.
 * @return
 */
bool EntityItem::isHardwareCluster()
{
    return IS_HARDWARE_CLUSTER;
}

bool EntityItem::isHardwareNode()
{
    return IS_HARDWARE_NODE;
}


/**
 * @brief EntityItem::isVector
 * @return
 */
bool EntityItem::isVector()
{
    return IS_VECTOR;
}






void EntityItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    //Get Render State.
    RENDER_STATE renderState = getRenderStateFromZoom(lod);

    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    VIEW_STATE viewState = getViewState();

    //Background
    if(renderState >= RS_BLOCK){
        QBrush headBrush = this->headerBrush;
        QBrush bodyBrush = this->bodyBrush;

        if(IS_READ_ONLY_SNIPPET){
            if(IS_READ_ONLY_DEF){
                bodyBrush = readOnlyDefBodyBrush;
                headBrush = readOnlyDefHeaderBrush;
            }else{
                bodyBrush = readOnlyBodyBrush;
                headBrush = readOnlyHeaderBrush;
            }
        }

        //Make the background transparent
        if((renderState > RS_BLOCK) && (viewState == VS_MOVING || viewState == VS_RESIZING)){
            if(isSelected() && isNodeOnGrid){
                QColor color = bodyBrush.color();
                color.setAlpha(90);
                bodyBrush.setColor(color);
            }
        }

        // this highlights this item if it is a hardware entity and the selected entity is connected to it
        if (isHardwareLink) {
            //bodyBrush.setColor(QColor(90,150,200));
            //headBrush.setColor(QColor(90,150,200));
            bodyBrush.setColor(Qt::darkBlue);
            headBrush.setColor(Qt::darkBlue);
        }

        if (isHighlighted()) {
            //bodyBrush.setColor(Qt::white);
            //bodyBrush.setColor(QColor(255,136,0));
            //headBrush.setColor(QColor(255,136,0));
            headBrush.setColor(Theme::theme()->getHighlightColor());
        }
        //Paint Background
        painter->setPen(Qt::NoPen);
        painter->setBrush(bodyBrush);
        painter->drawRect(boundingRect());


        if(isSelected() && renderState == RS_BLOCK){
            headBrush.setColor(selectedPen.color());
        }


        painter->setBrush(headBrush);
        painter->drawRect(headerRect());




        //Draw the boundary.
        //if(renderState >= RS_REDUCED || isSelected() || hasHardwareWarning){
            if(renderState != RS_BLOCK){
                //Setup the Pen
                QPen  pen = this->pen;

                if(isSelected()){
                    pen = this->selectedPen;
                    pen.setWidthF(selectedPenWidth);
                }

                if (hasHardwareWarning) {
                    pen.setWidthF(selectedPenWidth);
                    pen.setStyle(Qt::DotLine);
                    pen.setColor(Qt::red);
                }

                if (isHighlighted()) {
                    pen.setStyle(Qt::DashLine);
                    pen.setColor(Qt::darkBlue);
                    pen.setWidthF(selectedPenWidth);
                }

                if(isHovered()){
                    if(pen.color() == Qt::black){
                        pen.setWidthF(selectedPenWidth);
                        pen.setColor(QColor(130,130,130));
                    }else{
                        pen.setColor(pen.color().lighter(130));
                    }
                }

                //Trace the boundary
                painter->setPen(pen);
                painter->setBrush(Qt::NoBrush);

                painter->drawRect(adjustRectForPen(boundingRect(), pen));
            }
        //}





        //New Code
        if(renderState > RS_BLOCK && drawGridLines()){
            painter->setPen(Qt::gray);
            QPen linePen = painter->pen();

            linePen.setStyle(Qt::DotLine);
            //linePen.setWidth(1);
            painter->setPen(linePen);
            painter->drawLines(getGridLines());

            linePen.setColor(headerBrush.color().darker(220));
            linePen.setStyle(Qt::DotLine);
            linePen.setWidthF(selectedPenWidth);

            painter->setPen(linePen);

            foreach(QRectF rect, getChildrenGridOutlines()){
                painter->drawRect(adjustRectForPen(rect, linePen));
            }
        }

        //Paint the Icon

        if(renderState > RS_BLOCK){
            paintPixmap(painter, lod, IP_CENTER, getIconPrefix(), getIconURL(), changeIcon);
        }
    }


    if(IS_READ_ONLY){
        paintPixmap(painter, lod, IP_TOPLEFT, "Actions", "Lock_Closed");
    }

    if(nodeMemberIsKey){
        paintPixmap(painter, lod, IP_CENTER_SMALL, "Actions", "Key");
    }

    if(renderState == RS_FULL){
        //If a Node has a Definition, paint a definition icon
        if(IS_READ_ONLY_DEF){
            paintPixmap(painter, lod, IP_TOPLEFT, "Actions", "Snippet");
        }else if (nodeKind == "HardwareCluster") {
            paintPixmap(painter, lod, IP_TOPLEFT, "Actions", "MenuCluster");
        }

        if(isInputParameter){
            paintPixmap(painter, lod, IP_TOPLEFT, "Actions", "Arrow_Forward");
        }
        if(isReturnParameter){
            paintPixmap(painter, lod, IP_TOPRIGHT, "Actions", "Arrow_Forward");
        }



        if(canNodeBeConnected){
            //Paint connect Icon
            paintPixmap(painter, lod, IP_TOPRIGHT, "Actions", "ConnectTo");

            QPen newPen(Qt::gray);
            newPen.setWidthF(0.5);
            painter->setPen(newPen);
            painter->drawEllipse(iconRect_TopRight());
        }


        if(hasChildren()){
            if(isExpanded()){
                paintPixmap(painter, lod, IP_BOT_RIGHT, "Actions", "Contract", true);
            }else{
                paintPixmap(painter, lod, IP_BOT_RIGHT, "Actions", "Expand", true);
            }
        }




        if(hasEditData){
            if(editableDataKey == "worker"){
                paintPixmap(painter, lod, IP_BOTLEFT, "Functions", workerKind);
            }else{
                paintPixmap(painter, lod, IP_BOTLEFT, "Data", editableDataKey);
            }

        }
    }

    if(renderState >= RS_REDUCED){
        //If this Node has a Deployment Warning, paint a warning Icon
        if(showDeploymentWarningIcon){
            paintPixmap(painter, lod, IP_TOPRIGHT, "Actions", "Warning");
        }
    }

}


bool EntityItem::mouseOverRightLabel(QPointF mousePosition)
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

bool EntityItem::mouseOverBotInputIcon(QPointF mousePosition)
{
    if(bottomInputItem && bottomInputItem->isVisible()){
        if(iconRect_BottomLeft().contains(mousePosition)){
            return true;
        }
    }
    return false;
}


bool EntityItem::mouseOverBotInput(QPointF mousePosition)
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

bool EntityItem::mouseOverDeploymentIcon(QPointF mousePosition)
{
    if(showDeploymentWarningIcon){
        return iconRect_TopRight().contains(mousePosition);
    }
    return false;
}

bool EntityItem::mouseOverDefinition(QPointF mousePosition)
{
    if (IS_READ_ONLY_DEF || nodeMemberIsKey || IS_READ_ONLY){
        return iconRect_TopLeft().contains(mousePosition);
    }
    return false;
}

bool EntityItem:: mouseOverIcon(QPointF mousePosition)
{

    if(iconRect().contains(mousePosition)){
        return true;
    }

    return false;
}

bool EntityItem::mouseOverTopBar(QPointF mousePosition)
{
    return headerRect().contains(mousePosition);
}


/**
 * @brief EntityItem::menuArrowPressed
 * @param mousePosition
 * @return
 */
bool EntityItem::mouseOverHardwareMenu(QPointF mousePosition)
{
    if (IS_HARDWARE_CLUSTER) {
        if(iconRect_TopLeft().contains(mousePosition)){
            return true;
        }
    }
    return false;
}

bool EntityItem::mouseOverConnect(QPointF mousePosition)
{
    if(canNodeBeConnected){
        return iconRect_TopRight().contains(mousePosition);
    }
    return false;
}

bool EntityItem::mouseOverExpand(QPointF mousePosition)
{
    return iconRect_BottomRight().contains(mousePosition);
}




/*
void EntityItem::addChildEdgeItem(EdgeItem *edge)
{
    if(!childEdges.contains(edge)){
        childEdges.append(edge);
    }
}

QList<EdgeItem *> EntityItem::getChildEdges()
{
    return childEdges;
}

void EntityItem::removeChildEdgeItem(EdgeItem *edge)
{
    childEdges.removeAll(edge);
}*/

/**
 * @brief EntityItem::isHidden
 * @return
 */
bool EntityItem::isHidden()
{
    return hidden;
}


qreal EntityItem::getWidth() const
{
    return getExpandedWidth();
}


qreal EntityItem::getHeight() const
{
    return getExpandedHeight();
}

qreal EntityItem::getCurrentWidth() const
{
    if(IS_EXPANDED_STATE){
        return expandedWidth;
    }else{
        return contractedWidth;
    }
}

qreal EntityItem::getCurrentHeight() const
{
    if(IS_EXPANDED_STATE){
        return expandedHeight;
    }else{
        return contractedHeight;
    }
}

double EntityItem::getExpandedWidth() const
{
    return expandedWidth;
}

double EntityItem::getExpandedHeight() const
{
    return expandedHeight;
}



void EntityItem::setCenterPos(QPointF pos)
{
    //pos is the new center Position.
    pos -= minimumRect().center();
    //QGraphicsItem::setPos(pos);

    setPos(pos);
}

QPointF EntityItem::centerPos()
{
    return pos() + minimumRect().center();
}


void EntityItem::adjustPos(QPointF delta)
{

    QPointF currentPos = pos();
    currentPos += delta;
    //hasSelectionMoved = true;
    setPos(currentPos);
}

void EntityItem::adjustSize(QSizeF delta)
{
    qreal newWidth = getExpandedWidth() + delta.width();
    qreal newHeight = getExpandedWidth() + delta.height();

    setExpandedWidth(newWidth);
    setExpandedHeight(newHeight);
}


void EntityItem::addChildOutline(EntityItem *EntityItem, QPointF gridPoint)
{
    prepareGeometryChange();
    QRectF nodeRect = EntityItem->boundingRect();
    QPointF deltaPos = gridPoint - EntityItem->minimumRect().center();
    nodeRect.translate(deltaPos);
    outlineMap.insert(EntityItem->getID(), nodeRect);
}

void EntityItem::removeChildOutline(int ID)
{
    if(outlineMap.contains(ID)){
        prepareGeometryChange();
        outlineMap.remove(ID);
    }
}




double EntityItem::getChildWidth()
{
    // to make the gap between the view aspects uniform
    return ITEM_SIZE;
}

double EntityItem::getChildHeight()
{
    return ITEM_SIZE;
}








/**
 * @brief EntityItem::setSelected Sets the EntityItem as selected, notifies connected edges to highlight.
 * @param selected
 */
void EntityItem::setSelected(bool selected)
{
    //If the current state of the EntityItem's selection is different to the one provided.
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
 * @brief EntityItem::setVisibilty Changes the Visibility of the EntityItem also notifies all connected edges.
 * @param visible
 */
void EntityItem::setVisibility(bool visible)
{
    bool emitChange = isVisibleTo(0) != visible;

    QGraphicsItem::setVisible(visible);

    emit setEdgeVisibility(visible);

    if(emitChange){
        emit visibilityChanged(visible);
    }
}



/**
 * @brief EntityItem::graphMLDataChanged This method is called when any connected Data object updates their value.
 * @param data The Data which has been changed.
 */
void EntityItem::dataChanged(QString keyName, QVariant data)
{
    bool boolValue = data.toBool();
    if(data.isNull()){
        boolValue = false;
    }
    if(keyName == "x" || keyName == "y"){
        qreal dataValue = data.toReal();
        //If data is related to the position of the EntityItem
        //Get the current center position.

        QPointF newCenter = centerPos();

        if(keyName == "x"){
            newCenter.setX(dataValue);
        }else if(keyName == "y"){
            newCenter.setY(dataValue);
        }

        //Update the center position.
        setCenterPos(newCenter);

    }else if(keyName == "width" || keyName == "height"){
        qreal dataValue = data.toReal();
        if(keyName == "width"){
            setExpandedWidth(dataValue);
        }else if(keyName == "height"){
            setExpandedHeight(dataValue);
        }
    }else if(keyName == "label"){
        QString dataValue = data.toString();
        //Update the Label
        updateTextLabel(dataValue);
    }else if(keyName == "architecture"){
        nodeHardwareArch = data.toString();
        update();
    }else if(keyName == "os"){
        nodeHardwareOS = data.toString();
        update();
    }else if(keyName == "type"){
        this->nodeType = data.toString();
    }else if(keyName == "localhost"){
        this->nodeHardwareLocalHost = boolValue;
        update();
    }else if(keyName == "key"){
        nodeMemberIsKey = boolValue;
        update();
    }else if(keyName == "isExpanded"){
        handleExpandState(boolValue);
    }else if(keyName == "readOnly"){
        IS_READ_ONLY = boolValue;
        update();
    }else if(keyName == "readOnlyDefinition"){
        IS_READ_ONLY_DEF = boolValue;
        update();
    }else if(keyName == "snippetID"){
        IS_READ_ONLY_SNIPPET = boolValue;
        update();
    }else if(keyName == "description"){
        //Use as tooltip.
        descriptionValue = data.toString();
    }else if(keyName == "worker"){
        //Use as tooltip.
        workerKind = data.toString();
    }else if(keyName == "operation"){
        //Use as tooltip.
        operationKind = data.toString();
    }

    if(keyName == editableDataKey){
        if(bottomInputItem){
            bottomInputItem->setValue(data.toString());

            if(getEntityAdapter()->isDataProtected(keyName)){
                bottomInputItem->setBrush(Qt::NoBrush);
            }else{
                bottomInputItem->setBrush(Qt::white);
            }
        }
    }
    if(keyName == statusModeDataKey){
        if(statusItem){
            statusItem->setValue(data.toString());
        }
    }
}


void EntityItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    VIEW_STATE viewState = getViewState();
    //Set the mouse down type to the type which matches the position.
    mouseDownType = getMouseOverType(event->scenePos());
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    switch(event->button()){
    case Qt::LeftButton:
        switch(viewState){
        case VS_NONE:
            //Goto VS_Selected
        case VS_SELECTED:
            //Enter Selected Mode.
            if (mouseDownType != MO_NONE) {
                if(getNodeView()){
                    getNodeView()->setStateSelected();
                    handleSelection(true, controlPressed);
                    //Store the previous position.
                    previousScenePosition = event->scenePos();

                    if(mouseDownType == MO_CONNECT){
                        getNodeView()->setStateConnect();
                    }
                }
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}


void EntityItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    VIEW_STATE viewState = getViewState();


    //Only if left button is down.
    if(event->buttons() & Qt::LeftButton){
        QPointF deltaPos = (event->scenePos() - previousScenePosition);

        switch (viewState){
        case VS_SELECTED:
            if(mouseDownType == MO_RESIZE || mouseDownType == MO_RESIZE_HOR || mouseDownType == MO_RESIZE_VER){
                if(getNodeView() && isResizeable()){
                    //Is resizing
                    getNodeView()->setStateResizing();
                }
            }else if(mouseDownType <= MO_TOPBAR && mouseDownType >= MO_ICON){
                if(getNodeView() && isMoveable()){
                    //Moving not resizing.
                    getNodeView()->setStateMoving();
                }
            }
            break;
        case VS_RESIZING:
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
        case VS_MOVING:
            emit EntityItem_MoveSelection(deltaPos);
            previousScenePosition = event->scenePos();
            break;
        default:
            break;
        }
    }
}

void EntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    VIEW_STATE viewState = getViewState();
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);



    //Only if left button is down.
    switch(event->button()){
    case Qt::LeftButton:{
        switch (viewState){
        case VS_MOVING:
            emit EntityItem_MoveFinished();
            break;
        case VS_RESIZING:
            emit NodeItem_ResizeFinished(getID());
            break;
        case VS_SELECTED:
            if(mouseDownType == MO_HARDWAREMENU){
                emit EntityItem_ShowHardwareMenu(this);
            }
            break;
        default:
            break;
        }
        break;
    }
    case Qt::RightButton:{
        switch (viewState){
        case VS_PAN:
            //IGNORE
        case VS_PANNING:
            //IGNORE
            break;
        default:
            //If we haven't Panned, we need to make sure this EntityItem is selected before the toolbar opens.1
            handleSelection(true, controlPressed);
            break;
        }
        break;
    }
    case Qt::MiddleButton:{
        switch (viewState){
        case VS_NONE:
        case VS_SELECTED:
            if(controlPressed){
                if(inMainView()){
                    //emit GraphMLItem_TriggerAction("Sorting Node");
                    //sort();
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



void EntityItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(inSubView()){
        return;
    }

    MOUSEOVER_TYPE mouseDblClickType = getMouseOverType(event->scenePos());

    RESIZE_TYPE rt = RESIZE;
    switch(event->button()){
    case Qt::LeftButton:{
        switch(mouseDblClickType){
        case MO_ICON:
            if(hasChildren()){
                if(isExpanded()){
                    GraphMLItem_TriggerAction("Contracted Node Item");
                }else{
                    GraphMLItem_TriggerAction("Expanded Node Item");
                }

                emit GraphMLItem_SetData(getID(), "isExpanded", !isExpandedState());
            }
            break;
        case MO_BOT_LABEL:

            break;
        case MO_RESIZE_HOR:
            if(mouseDblClickType == MO_RESIZE_HOR)
                rt = HORIZONTAL_RESIZE;
            //Continue
        case MO_RESIZE_VER:
            if(mouseDblClickType == MO_RESIZE_VER)
                rt = VERTICAL_RESIZE;
            //Continue
        case MO_RESIZE:
            if(hasChildren()){
                GraphMLItem_TriggerAction("Optimizes Size of EntityItem");
                resizeToOptimumSize(rt);
                updateSizeInModel();
            }
            break;

        case MO_ITEM:
            // added this to re-center aspect when double-clicked
            GraphMLItem_SetCentered(this);
            break;

        case MO_MODEL_TL:
            emit EntityItem_Model_AspectToggled(0);
            break;
        case MO_MODEL_TR:
            emit EntityItem_Model_AspectToggled(1);
            break;
        case MO_MODEL_BR:
            emit EntityItem_Model_AspectToggled(2);
            break;
        case MO_MODEL_BL:
            emit EntityItem_Model_AspectToggled(3);
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






void EntityItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QString tooltip = "";
    QCursor cursor;
    MOUSEOVER_TYPE hoverType = getMouseOverType(event->scenePos());


    switch(hoverType){

    case MO_CONNECT:
        cursor = Qt::CrossCursor;
        tooltip = "Click and drag to connect entity.";
        break;
    case MO_ICON:
        if(hasChildren()){
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
        if(IS_READ_ONLY_DEF){
            tooltip = "This Entity is a Read-Only Snippet definition.";
        }else if(IS_READ_ONLY){
            tooltip = "This Entity is in Read-Only Mode. You can only make visual changes.";
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
    case MO_BOT_LABEL_ICON:
        cursor = Qt::WhatsThisCursor;
        tooltip = descriptionValue;
        break;
    default:
        break;
    }


    setToolTip(tooltip);

    if(cursor.shape() != Qt::ArrowCursor){
        setCursor(cursor);
    }else{
        unsetCursor();
    }

    GraphMLItem::hoverMoveEvent(event);
}

void EntityItem::updateErrorState()
{
    NodeAdapter* node = getNodeAdapter();
    if(node->isBehaviourAdapter()){
        BehaviourNodeAdapter* bA = (BehaviourNodeAdapter*)node;

        if(bA->needsLeftEdge()){
            notificationItem->setErrorType(ET_CRITICAL, "Entity requires workload edge.");
        }else{
            //Clear the notification.
            notificationItem->setErrorType(ET_OKAY);
        }
        setNodeConnectable(bA->needsConnection());
    }else if(IS_IMPL_OR_INST){
        if(HAS_DEFINITION){
            //Clear the notification.
            notificationItem->setErrorType(ET_OKAY);
        }else{
            //Clear the notification.
            notificationItem->setErrorType(ET_CRITICAL, "Instance/Impl isn't connected to a Definition.");
        }
    }
}


/*void EntityItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
   // handleHover(false);

    //Unset the cursor
    unsetCursor();
}*/

void EntityItem::updateTextVisibility()
{
    bool showRightLabel = false;
    bool showTopLabel = true;
    bool showBottomLabel = !editableDataKey.isEmpty();

    if(isExpanded()){
        if(!getChildren().isEmpty()){
            showRightLabel = true;
        }
    }

    if(getCurrentWidth() > ACTUAL_ITEM_SIZE + GRID_PADDING_SIZE){
        if(!rightLabelInputItem->isTruncated()){
            showTopLabel = false;
        }
    }

    if (topLabelInputItem && rightLabelInputItem) {
        topLabelInputItem->setVisible(showTopLabel);
        rightLabelInputItem->setVisible(showRightLabel);

        bottomInputItem->setVisible(showBottomLabel);
    }
}

void EntityItem::setWarning(bool warning, QString warningTooltip)
{
    hasWarning = warning;
    this->warningTooltip = warningTooltip;
}


/**
 * @brief EntityItem::updateDisplayedChildren
 * @param viewMode
 */
void EntityItem::updateDisplayedChildren(int viewMode)
{
    // if this item is not a HardwareCLuster, do nothing
    if (!IS_HARDWARE_CLUSTER) {
        return;
    }

    // if any of the menu items are not constructed, do nothing
    if (!allChildren || !connectedChildren || !unConnectedChildren) {
        return;
    }

    // if viewMode = -1, it means that an edgeConstructed signal was sent
    // need to update children HardwareNodes' visibily
    if (viewMode == -1) {
        viewMode = CHILDREN_VIEW_MODE;
    }

    QList<EntityItem*> childrenItems = getChildEntityItems();

    allChildren->setChecked(false);
    connectedChildren->setChecked(false);
    unConnectedChildren->setChecked(false);

    switch (viewMode) {
    case ALL:
        // show all HarwareNodes
        allChildren->setChecked(true);
        foreach (EntityItem* item, childrenItems) {
            item->setHidden(false);
        }
        break;
    case CONNECTED:
        // show connected HarwareNodes
        connectedChildren->setChecked(true);
        foreach (EntityItem* item, childrenItems) {
            if (item->getNodeAdapter()) {
                bool hasEdges = item->getNodeAdapter()->edgeCount() <= 0;
                item->setHidden(hasEdges);
            }
        }
        break;
    case UNCONNECTED:
        // show unconnected HarwareNodes
        unConnectedChildren->setChecked(true);
        foreach (EntityItem* item, childrenItems) {
            if (item->getNodeAdapter()) {
                bool hasEdges = item->getNodeAdapter()->edgeCount() <= 0;
                item->setHidden(!hasEdges);
            }
        }
        break;
    default:
        return;
    }

    CHILDREN_VIEW_MODE = viewMode;
    sortTriggerAction = false;
    sortChildren();
    sortTriggerAction = true;
}


QRectF EntityItem::smallIconRect() const
{
    qreal iconSize = SMALL_ICON_RATIO * contractedWidth;
    return QRectF(0, 0, iconSize, iconSize);
}


/**
 * @brief EntityItem::iconRect
 * @return - The QRectF which represents the position for the Icon
 */
QRectF EntityItem::iconRect() const
{
    qreal iconSize = ICON_RATIO * contractedWidth;
    //if(getRenderState() <= RS_MINIMAL){
    //    iconSize = contractedWidth;
    //}

    //Construct a Rectangle to represent the icon size at the origin
    QRectF icon = QRectF(0, 0, iconSize, iconSize);


    QPointF centerPoint = minimumRect().center();

    icon.moveCenter(centerPoint);
    return icon;
}

QRectF EntityItem::textRect_Top() const
{
    qreal textHeight = TOP_LABEL_RATIO * contractedHeight;
    qreal textWidth = boundingRect().width();
    QRectF rect;
    rect.setHeight(textHeight);
    rect.setWidth(textWidth);
    rect.moveLeft(boundingRect().left());
    rect.moveBottom(-getItemMargin()/2);
    return rect;
}

QRectF EntityItem::textRect_Right() const
{
    QRectF icon = iconRect();

    qreal textHeight = icon.height() / 3;
    qreal textWidth = expandedBoundingRect().right() - icon.right() - getItemMargin();
    return QRectF(icon.right(), icon.center().y() - textHeight/2, textWidth, textHeight);
}

QRectF EntityItem::textRect_Bot() const
{
    qreal textHeight = iconRect_BottomLeft().height();
    qreal textWidth = iconRect_BottomRight().left() - iconRect_BottomLeft().right();
    QRectF rect;
    rect.setHeight(textHeight);
    rect.setWidth(textWidth);

    rect.moveBottomLeft(iconRect_BottomLeft().bottomRight());
    return rect;
}

QRectF EntityItem::statusRect_Left() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    qreal rWidth = SMALL_ICON_RATIO * contractedWidth;

    QRectF headRect = minimumRect();
    headRect.setWidth(rWidth);

    return headRect;
}

QRectF EntityItem::iconRect_TopMid() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    qreal itemMargin = getItemMargin();
    iconRect.moveTopLeft(QPointF(itemMargin + (getCurrentWidth()/2) - (iconRect.width() /2), itemMargin));

    return iconRect;

}

QRectF EntityItem::iconRect_Center() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    iconRect.moveCenter(minimumRect().center());

    return iconRect;
}



/**
 * @brief EntityItem::lockIconRect
 * @return The QRectF which represents the position for the Lock Icon
 */
QRectF EntityItem::iconRect_TopLeft() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    qreal itemMargin = getItemMargin();
    iconRect.moveTopLeft(QPointF(itemMargin, itemMargin));

    return iconRect;
}

QRectF EntityItem::iconRect_TopRight() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    qreal itemMargin = (getItemMargin());
    iconRect.moveTopRight(QPointF(itemMargin + getCurrentWidth(), itemMargin));
    return iconRect;
}


QRectF EntityItem::iconRect_BottomLeft() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    QRectF iconRect = smallIconRect();

    //Translate to move the icon to its position
    qreal itemMargin = (getItemMargin());
    iconRect.moveBottomLeft(QPointF(itemMargin, itemMargin + contractedHeight));
    return iconRect;
}

QRectF EntityItem::iconRect_BottomRight() const
{
    //Construct a Rectangle to represent the icon size at the origin.
    qreal iconSize = MARGIN_RATIO * contractedWidth;
    QRectF iconRect =  QRectF(0, 0, iconSize, iconSize);

    //Translate to move the icon to its position
    qreal itemMargin = (getItemMargin());
    iconRect.moveBottomRight(QPointF(itemMargin + getCurrentWidth(), itemMargin + getCurrentHeight()));
    return iconRect;
}

QRectF EntityItem::getImageRect(EntityItem::IMAGE_POS pos) const
{
    switch(pos){
    case IP_BOTLEFT:
        return iconRect_BottomLeft();
    case IP_TOPRIGHT:
        return iconRect_TopRight();
    case IP_TOPMID:
        return iconRect();
    case IP_TOPLEFT:
        return iconRect_TopLeft();
    case IP_BOT_RIGHT:
        return iconRect_BottomRight();
    case IP_CENTER:
        return iconRect();
    case IP_CENTER_SMALL:
        return iconRect_Center();
    default:
        return QRectF();
    }
}

void EntityItem::setImage(EntityItem::IMAGE_POS pos, QPixmap image)
{
    imageMap[pos] = image;
}



bool EntityItem::isInResizeMode()
{
    return currentResizeMode != NO_RESIZE;
}

bool EntityItem::isMoveable()
{
    return true;
}

bool EntityItem::isResizeable()
{
    return isSelected() && isExpanded() && hasChildren();
}

void EntityItem::setExpandedWidth(qreal w)
{

    qreal minWidth = childrenBoundingRect().right();
    minWidth = qMax(minWidth, contractedWidth);
    w = qMax(w, minWidth);

    if(w != expandedWidth){
        expandedWidth = w;

        if(IS_EXPANDED_STATE){
            prepareGeometryChange();
            emit GraphMLItem_SizeChanged();
        }
        updateTextLabel();
    }
}

void EntityItem::setExpandedHeight(qreal h)
{
    qreal minHeight = childrenBoundingRect().bottom();
    minHeight = qMax(minHeight, contractedHeight);
    h = qMax(h, minHeight);

    if(h != expandedHeight){
        expandedHeight = h;

        if(IS_EXPANDED_STATE){
            prepareGeometryChange();
            emit GraphMLItem_SizeChanged();
        }
    }

}

bool EntityItem::isExpandedState()
{
    return IS_EXPANDED_STATE;
}





void EntityItem::setWidth(qreal w)
{
    setExpandedWidth(w);
}


void EntityItem::setHeight(qreal h)
{
    setExpandedHeight(h);
}

void EntityItem::setSize(qreal w, qreal h)
{
    setExpandedWidth(w);
    setExpandedHeight(h);
}


/**
 * @brief EntityItem::calculateGridlines Calculates the horizontal and vertical gridlines to be drawn by paint.
 */
void EntityItem::calculateGridlines()
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




bool EntityItem::isAncestorSelected()
{
    if(parentEntityItem){
        if(parentEntityItem->isSelected()){
            return true;
        }else{
            return parentEntityItem->isAncestorSelected();
        }
    }
    return false;
}


/**
 * @brief EntityItem::setGridVisible Sets the grid as visible.
 * @param visible
 */
void EntityItem::setGridVisible(bool visible)
{
    if(isGridVisible != visible){
        isGridVisible = visible;
        update();
    }
}


void EntityItem::updateTextLabel(QString newLabel)
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

    topLabelInputItem->updatePosSize(textRect_Top());
    rightLabelInputItem->updatePosSize(textRect_Right());

    bottomInputItem->updatePosSize(textRect_Bot());

    statusItem->setCircleCenter(boundingRect().topRight());


    QPointF notificationIconPos =  boundingRect().topRight() -  QPointF(0,statusItem->boundingRect().height() - getItemMargin());// - statusItem->boundingRect().bottomRight();
    notificationItem->setPos(notificationIconPos);
    notificationItem->setRotation(notificationItem->getAngle());
    updateTextVisibility();
}





void EntityItem::setupBrushes()
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
        if(!parentEntityItem || (parentEntityItem && parentEntityItem->getNodeKind().endsWith("Definitions"))){
            color = QColor(233,234,237);
        }else{
            if(parentEntityItem){
                color = parentEntityItem->getBackgroundColor().darker(110);

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
    pen.setJoinStyle(Qt::MiterJoin);
    //pen.setColor(Qt::darkGray);
    selectedPen.setColor(Qt::blue);
    selectedPen.setWidth(1);
    selectedPen.setJoinStyle(Qt::MiterJoin);

    //Set up ReadOnly Bruhs

    QColor blendColor = QColor(100,149,237);
    qreal blendFactor = .2;
    QColor bColor = bodyBrush.color();

    bColor.setBlue(blendFactor * blendColor.blue() + (1 - blendFactor) * bColor.blue());
    bColor.setRed(blendFactor * blendColor.red() + (1 - blendFactor) * bColor.red());
    bColor.setGreen(blendFactor * blendColor.green() + (1 - blendFactor) * bColor.green());


    readOnlyBodyBrush = QBrush(bColor);
    blendFactor = .6;

    bColor = headerBrush.color();
    bColor.setBlue(blendFactor * blendColor.blue() + (1 - blendFactor) * bColor.blue());
    bColor.setRed(blendFactor * blendColor.red() + (1 - blendFactor) * bColor.red());
    bColor.setGreen(blendFactor * blendColor.green() + (1 - blendFactor) * bColor.green());
    readOnlyHeaderBrush = QBrush(bColor);



    blendColor = QColor(222,184,135);
    blendFactor = .6;
    bColor = headerBrush.color();
    bColor.setBlue(blendFactor * blendColor.blue() + (1 - blendFactor) * bColor.blue());
    bColor.setRed(blendFactor * blendColor.red() + (1 - blendFactor) * bColor.red());
    bColor.setGreen(blendFactor * blendColor.green() + (1 - blendFactor) * bColor.green());

    errorHeaderBrush = QBrush(bColor);


    blendColor = QColor(222,184,135);
    blendFactor = .2;
    bColor = bodyBrush.color();

    bColor.setBlue(blendFactor * blendColor.blue() + (1 - blendFactor) * bColor.blue());
    bColor.setRed(blendFactor * blendColor.red() + (1 - blendFactor) * bColor.red());
    bColor.setGreen(blendFactor * blendColor.green() + (1 - blendFactor) * bColor.green());


    readOnlyDefBodyBrush = QBrush(bColor);
    blendFactor = .6;

    bColor = headerBrush.color();
    bColor.setBlue(blendFactor * blendColor.blue() + (1 - blendFactor) * bColor.blue());
    bColor.setRed(blendFactor * blendColor.red() + (1 - blendFactor) * bColor.red());
    bColor.setGreen(blendFactor * blendColor.green() + (1 - blendFactor) * bColor.green());

    readOnlyDefHeaderBrush = QBrush(bColor);

}


void EntityItem::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}


void EntityItem::setPos(const QPointF pos)
{
    QPointF newPos = pos;

    //is the new position different.
    if(newPos != this->pos()){
        prepareGeometryChange();
        QGraphicsItem::setPos(newPos);



        //Need to check if we are over the grid.
        isOverGrid(getMinimumRectCenterPos());



        emit GraphMLItem_PositionChanged();
    }
}



/**
 * @brief EntityItem::setupChildrenViewOptionMenu
 */
void EntityItem::setupChildrenViewOptionMenu()
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
void EntityItem::childUpdated()
{
    if(!getEntityAdapter()){
        return;
    }

    QRectF childRect = childrenBoundingRect();

    //Maximize on the current size in the Model and the minimum child rectangle
    if(childRect.right() > getExpandedWidth()){
        setExpandedWidth(childRect.right());
    }

    if(childRect.bottom() > getExpandedHeight()){
        setExpandedHeight(childRect.bottom());
    }
}

/**
 * @brief EntityItem::setupLabel
 * This sets up the font and size of the label.
 */
void EntityItem::setupLabel()
{

    qreal topFontSize = TOP_LABEL_RATIO * contractedHeight;
    qreal rightFontSize = RIGHT_LABEL_RATIO * contractedHeight;
    qreal botFontSize = BOTTOM_LABEL_RATIO * contractedHeight;

    bottomInputItem = new InputItem(this, "", false);
    topLabelInputItem = new InputItem(this,"", false);
    rightLabelInputItem = new InputItem(this, "", false);
    statusItem = new StatusItem(this);
    statusItem->setBackgroundColor(QColor(0,150,150));
    notificationItem = new NotificationItem(this);
    notificationItem->setBackgroundColor(QColor(255,204,51));

    //Setup external Label
    topLabelInputItem->setAcceptHoverEvents(true);
    topLabelInputItem->setToolTipString("Double click to edit label.");
    topLabelInputItem->setCursor(Qt::IBeamCursor);

    //Setup external statusItem
    statusItem->setAcceptHoverEvents(true);
    statusItem->setToolTipString("Click to edit field.");

    notificationItem->setAcceptHoverEvents(true);




    topLabelInputItem->setAlignment(Qt::AlignCenter);
    rightLabelInputItem->setAlignment(Qt::AlignLeft  | Qt::AlignVCenter);
    bottomInputItem->setAlignment(Qt::AlignLeft  | Qt::AlignVCenter);

    statusItem->setAlignment(Qt::AlignCenter);

    //Hide it.
    bottomInputItem->setVisible(false);

    QFont textFont;
    textFont.setPixelSize(rightFontSize);
    rightLabelInputItem->setFont(textFont);
    statusItem->setFont(textFont);
    textFont.setPixelSize(topFontSize);
    topLabelInputItem->setFont(textFont);
    textFont.setPixelSize(botFontSize);
    //textFont.setItalic(true);
    bottomInputItem->setFont(textFont);


    connect(rightLabelInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(labelEditModeRequest()));
    connect(topLabelInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(labelEditModeRequest()));
    connect(bottomInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(labelEditModeRequest()));
    connect(statusItem, SIGNAL(statusItem_EditModeRequested()), this, SLOT(labelEditModeRequest()));

    connect(topLabelInputItem, SIGNAL(InputItem_ValueChanged(QString)),this, SLOT(dataChanged(QString)));
    connect(rightLabelInputItem, SIGNAL(InputItem_ValueChanged(QString)),this, SLOT(dataChanged(QString)));
    connect(bottomInputItem, SIGNAL(InputItem_ValueChanged(QString)),this, SLOT(dataChanged(QString)));
    connect(statusItem, SIGNAL(InputItem_ValueChanged(QString)),this, SLOT(dataChanged(QString)));


    connect(topLabelInputItem, SIGNAL(InputItem_HasFocus(bool)), this, SIGNAL(EntityItem_HasFocus(bool)));
    connect(rightLabelInputItem, SIGNAL(InputItem_HasFocus(bool)), this, SIGNAL(EntityItem_HasFocus(bool)));
    connect(bottomInputItem, SIGNAL(InputItem_HasFocus(bool)), this, SIGNAL(EntityItem_HasFocus(bool)));
    connect(statusItem, SIGNAL(InputItem_HasFocus(bool)), this, SIGNAL(EntityItem_HasFocus(bool)));

    QPointF bottomLabelPos = iconRect_BottomLeft().topRight();
    QPointF expandedLabelPos = expandedLabelRect().topLeft() - QPointF(0, rightLabelInputItem->boundingRect().height() /2);
    QPointF statusIconPos = boundingRect().topRight() - statusItem->boundingRect().center();

    rightLabelInputItem->setPos(expandedLabelPos);
    bottomInputItem->setPos(bottomLabelPos);
    topLabelInputItem->setPos(bottomLabelPos - QPointF(0 , bottomInputItem->boundingRect().height()));
    statusItem->setPos(statusIconPos);

    QPointF notificationIconPos =  boundingRect().topRight() - statusItem->boundingRect().bottomRight();
    notificationItem->setPos(notificationIconPos);



}




/**
 * @brief EntityItem::setupGraphMLConnections
 */
void EntityItem::setupDataConnections()
{
    listenForData("x");
    listenForData("y");
    listenForData("height");
    listenForData("width");
    listenForData("label");
    listenForData("type");
    listenForData("isExpanded");

    listenForData("readOnly");
    listenForData("readOnlyDefinition");
    listenForData("snippetID");


    listenForData("description");

    if(nodeKind == "HardwareNode"){
        listenForData("os");
        listenForData("architecture");
        listenForData("localhost");
    }else if(nodeKind == "Member"){
        listenForData("key");
    }else if(nodeKind == "Process"){
        listenForData("worker");
        listenForData("operation");
    }

    NodeAdapter* node = getNodeAdapter();
    if(node){
        connect(node, SIGNAL(edgeAdded(int,Edge::EDGE_CLASS)), this, SLOT(edgeAdded(int,Edge::EDGE_CLASS)));
        connect(node, SIGNAL(edgeRemoved(int,Edge::EDGE_CLASS)), this, SLOT(edgeRemoved(int,Edge::EDGE_CLASS)));
    }
}


QPointF EntityItem::isOverGrid(const QPointF centerPosition)
{
    if(!isGridModeOn() || !getParentNodeItem()){
        return QPointF();
    }

    QPointF gridPoint = getParentNodeItem()->getClosestGridPoint(centerPosition);

    //Calculate the distance between the centerPosition and the closestGrid
    qreal distance = QLineF(centerPosition, gridPoint).length();

    //If the distance is less than the SNAP_PERCENTAGE
    if((distance / contractedWidth) <= SNAP_PERCENTAGE){
        getParentNodeItem()->showChildGridOutline(this, gridPoint);
    }else{
        getParentNodeItem()->hideChildGridOutline(getID());
        gridPoint = QPointF();
    }
    return gridPoint;
}





void EntityItem::toggleGridLines(bool on)
{
    if(on != GRIDLINES_ON){
        GRIDLINES_ON = on;
        if(on){
            calculateGridlines();
        }
    }
}




/*void EntityItem::snapToGrid()
{
    if(parentEntityItem && isVisible()){
        QPointF gridPosition = parentEntityItem->getClosestGridPoint(centerPos());

        setCenterPos(gridPosition);
    }
}*/



/**
 * @brief EntityItem::menuClosed
 */
void EntityItem::menuClosed()
{
    emit EntityItem_lockMenuClosed(this);
}


/**
 * @brief EntityItem::updateChildrenViewMode
 * @param viewMode
 */
void EntityItem::updateChildrenViewMode(int viewMode)
{
    updateDisplayedChildren(viewMode);
}


/**
 * @brief EntityItem::hardwareClusterMenuItemPressed
 */
void EntityItem::hardwareClusterMenuItemPressed()
{
    // if this item is not a HardwareCluster, do nothing
    if (!IS_HARDWARE_CLUSTER) {
        return;
    }

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

        emit EntityItem_HardwareMenuClicked(value);
    }
}


/**
 * @brief EntityItem::getHardwareClusterChildrenViewMode
 * @return
 */
int EntityItem::getHardwareClusterChildrenViewMode()
{
    return CHILDREN_VIEW_MODE;
}


/**
 * @brief EntityItem::highlightHardwareLink
 * @param nodeItem
 */
void EntityItem::highlightHardwareLink(NodeItem *nodeItem)
{
    if (isHardwareNode() || isHardwareCluster()) {
        if (nodeItem == this) {
            qDebug() << "HIGHLIGHT";
            isHardwareLink = true;
        } else {
            qDebug() << "CLEAR HIGHLIGHT";
            isHardwareLink = false;
        }
        update();
    }
}


/**
 * @brief EntityItem::themeChanged
 * @param theme
 */
void EntityItem::themeChanged(VIEW_THEME theme)
{
    if (IS_HARDWARE_CLUSTER && childrenViewOptionMenu) {

        QString bgColor = "rgba(240,240,240,250);";
        QString textColor = "black;";
        QString checkedColor = "green;";

        switch (theme) {
        case VT_DARK_THEME:
            bgColor = "rgba(130,130,130,250);";
            textColor = "white;";
            checkedColor = "yellow;";
        default:
            break;
        }

        childrenViewOptionMenu->setStyleSheet("QMenu{ background-color:" + bgColor + "}" +
                                              "QRadioButton {"
                                              "padding: 8px 10px 8px 8px;"
                                              "color:" + textColor +
                                              "}"
                                              "QRadioButton::checked {"
                                              "font-weight: bold; "
                                              "color:" + checkedColor +
                                              "}");
    }
}





void EntityItem::retrieveData()
{

    if(!getEntityAdapter()){
        return;
    }


    updateData("width");
    updateData("height");
    updateData("x");
    updateData("y");
    updateData("label");
    updateData("type");
}


/**
 * @brief EntityItem::getChildEntityItems
 * @return
 */
QList<EntityItem *> EntityItem::getChildEntityItems()
{
    QList<EntityItem*> insertOrderList;

    foreach(GraphMLItem* item, getChildren()){
        if(item->isEntityItem()){
            insertOrderList.append((EntityItem*)item);
        }
    }
    return insertOrderList;

}


qreal EntityItem::getGridSize() const
{
    return GRID_SIZE;
}

qreal EntityItem::getGridGapSize() const
{
    return GRID_PADDING_SIZE;
}









/**
 * @brief EntityItem::showHardwareIcon
 * When the deployment view is on, this shows the red hardware icon denoting
 * that this item has a child that is deployed to a different hardware node.
 * @param show
 */
void EntityItem::showHardwareIcon(bool show)
{
    showDeploymentWarningIcon = show;
    //update(deploymentIconRect());
}


/**
 * @brief EntityItem::deploymentView
 * @param on
 * @param selectedItem
 * @return
 */
QList<EntityItem*> EntityItem::deploymentView(bool on, EntityItem *)
{
    QList<EntityItem*> chlidrenDeployedToDifferentNode;

    if (on) {
        /*

        // get the hardware node that this item is deployed to
        foreach (Edge* edge, getNodeAdapter()->getEdges(0)) {
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
        foreach (EntityItem* childItem, getChildEntityItems()) {
            foreach (Edge* edge, childItem->getNodeAdapter()->getEdges(0)) {
                if (edge->isDeploymentLink() && edge->getDestination() != deploymentLink) {
                    if (selectedItem && selectedItem == this) {
                        childItem->setHardwareHighlighting(true);
                        //childItem->highlightEntityItem(true);
                    }
                    chlidrenDeployedToDifferentNode.append(childItem);
                    break;
                }
            }
        }
        */

        // if there are children deployed to a different node, show red hardware icon
        if (chlidrenDeployedToDifferentNode.isEmpty()) {
            showHardwareIcon(false);
        } else {
            showHardwareIcon(true);
        }

    } else {

        // remove highlight and hide the red hradware icon
        foreach (EntityItem* childItem, getChildEntityItems()) {
            childItem->setHardwareHighlighting(false);
            //childItem->highlightEntityItem(false);
        }

        showHardwareIcon(false);
    }

    // need to update here otherwise the visual changes aren't applied till the mouse is moved
    update();

    return chlidrenDeployedToDifferentNode;
}


/**
 * @brief EntityItem::getChildrenViewMode
 * @return
 */
int EntityItem::getChildrenViewMode()
{
    return CHILDREN_VIEW_MODE;
}


/**
 * @brief EntityItem::forceExpandParentItem
 */
void EntityItem::forceExpandParentItem()
{
    QList<EntityItem*> parentItems;
    EntityItem* parentItem = getParentEntityItem();
    while (parentItem) {
        if (!parentItem->isExpanded()) {
            //emit GraphMLItem_SetData(parentItem->getID(), "isExpanded", true);
            parentItems.append(parentItem);
        }
        parentItem = parentItem->getParentEntityItem();
    }

    for (int i = parentItems.count() - 1; i >= 0; i--) {
        EntityItem* pi = parentItems.at(i);
        emit GraphMLItem_SetData(pi->getID(), "isExpanded", true);
    }
}

void EntityItem::gotDefinition(bool def)
{
    HAS_DEFINITION = def;
    updateErrorState();
}

void EntityItem::edgeAdded(int ID, Edge::EDGE_CLASS edgeClass)
{
    Q_UNUSED(ID);
    Q_UNUSED(edgeClass);
    updateErrorState();
}

void EntityItem::edgeRemoved(int ID, Edge::EDGE_CLASS edgeClass)
{
    Q_UNUSED(ID);
    Q_UNUSED(edgeClass);
    updateErrorState();
}



void EntityItem::labelEditModeRequest()
{
    if(inSubView()){
        return;
    }
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());
    StatusItem* statusItem = qobject_cast<StatusItem*>(QObject::sender());
    if (inputItem){
        QString dataKey = "label";
        bool comboBox = false;
        if (inputItem == bottomInputItem) {
            dataKey = editableDataKey;
            comboBox = editableDataDropDown;
        }
        if(statusItem == this->statusItem){
            dataKey = statusModeDataKey;
        }
        if (isDataEditable(dataKey)) {
            if (comboBox) {
                QString currentValue = bottomInputItem->getValue();
                QPointF botLeft = inputItem->sceneBoundingRect().bottomLeft();
                QPointF botRight = inputItem->sceneBoundingRect().bottomRight();
                QLineF botLine = QLineF(botLeft,botRight);
                getNodeView()->showDropDown(this, botLine, editableDataKey, currentValue);
            } else {
                if(statusItem){
                    statusItem->setEditMode(true);
                }else{
                    inputItem->setEditMode(true);
                }
            }
        }
    }
}

void EntityItem::dataChanged(QString dataValue)
{
    //Determine the sender
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());
    if(inputItem && getEntityAdapter()){

        QString keyValue = "label";

        if(inputItem == bottomInputItem){
            keyValue = editableDataKey;
        }
        if(inputItem == statusItem){
            keyValue = statusModeDataKey;
        }

        if(isDataEditable(keyValue)){
            emit GraphMLItem_TriggerAction("Data: " + keyValue + " Changed");
            setData(keyValue, dataValue);
        }
    }
}



void EntityItem::zoomChanged(qreal currentZoom)
{
    setRenderState(getRenderStateFromZoom(currentZoom));

    updateTextVisibility();

    //update penWidth.
    selectedPenWidth = qMax(SELECTED_LINE_WIDTH / currentZoom, 1.0);

    //Call base class
    GraphMLItem::zoomChanged(currentZoom);
}

GraphMLItem::RENDER_STATE EntityItem::getRenderStateFromZoom(qreal zoom)
{
    if(zoom >= 1.0){
        return RS_FULL;
    }else if(zoom >= (2.0/3.0)){
        return RS_REDUCED;
    }else if(zoom >= (1.0/3.0)){
        return RS_MINIMAL;
    }else{
        return RS_BLOCK;
    }
}

/**
 * @brief EntityItem::getChildrenViewOptionMenu
 * @return
 */
QMenu *EntityItem::getChildrenViewOptionMenu()
{
    return childrenViewOptionMenu;
}


/**
 * @brief EntityItem::geChildrenViewOptionMenuSceneRect
 * @return
 */
QRectF EntityItem::geChildrenViewOptionMenuSceneRect()
{
    if (IS_HARDWARE_CLUSTER) {
        QRectF menuButtonRect = mapRectToScene(iconRect_TopLeft());
        return menuButtonRect;
    }
    return QRectF();
}




QString EntityItem::getIconURL()
{
    QString imageURL = nodeKind;

    if (IS_HARDWARE_NODE) {
        if (nodeHardwareLocalHost) {
            imageURL = "Localhost";
        } else {
            imageURL = nodeHardwareOS.remove(" ") + "_" + nodeHardwareArch;
        }
    } else if (IS_VECTOR) {
        imageURL = vectorIconURL;
    } else if (nodeKind.endsWith("Parameter")) {
        return nodeLabel;
    } else if(nodeKind == "Process"){
        return operationKind;
    }

    return imageURL;
}

QString EntityItem::getIconPrefix()
{
    QString imageURL = nodeKind;
    if(nodeKind.endsWith("Parameter")){
        return "Data";
    }else if(nodeKind == "Process"){
        return "Functions";
    }
    return "Items";

}

void EntityItem::paintPixmap(QPainter *painter, qreal lod, EntityItem::IMAGE_POS pos, QString alias, QString imageName, bool update)
{
    QRectF place = getImageRect(pos);
    QPixmap image = imageMap[pos];

    if(getNodeView() && (image.isNull() || update)){
        //Try get the image the user asked for.
        image = getNodeView()->getImage(alias, imageName);

        if(image.isNull() && workerKind != ""){
            //Try get the Icon for the worker otherwise.
            image = getNodeView()->getImage("Functions", workerKind);
        }

        if(image.isNull() && operationKind != ""){
            //Use the default icon for the Process.
            image = getNodeView()->getImage("Items", "Process");
        }

        if(image.isNull() && nodeType != ""){
            //Look for a Data icon.
            image = getNodeView()->getImage("Data", nodeType);
        }

        if(image.isNull()){
            //Use a help icon.
            image = getNodeView()->getImage("Actions", "Help");
        }
        imageMap[pos] = image;
    }
    int width = place.width()* lod * 2;
    int height = place.height() * lod * 2;
    QPixmap newPixmap = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter->drawPixmap(place.x(), place.y(), place.width(), place.height(), newPixmap);

    if (changeIcon) {
        emit entityItem_iconChanged();
        changeIcon = false;
    }
}



/**
 * @brief EntityItem::getChildKind
 * This returns a list of kinds of all this item's children.
 * @return
 */
QStringList EntityItem::getChildrenKind()
{
    QStringList childrenKinds;
    Node *node = dynamic_cast<Node*>(getEntityAdapter());
    if (node) {

        foreach(Node* child, node->getChildren(0)){
            childrenKinds.append(child->getDataValue("kind").toString());
        }
    }
    return childrenKinds;
}




QSizeF EntityItem::getModelSize()
{

    float graphmlHeight = 0;
    float graphmlWidth = 0;

    graphmlWidth = getDataValue("width").toDouble();
    graphmlHeight = getDataValue("height").toDouble();




    return QSizeF(graphmlWidth, graphmlHeight);
}

bool EntityItem::canHover()
{
    if(getNodeKind().endsWith("Definitions") || getNodeKind() == "Model"){
        return false;
    }
    return GraphMLItem::canHover();
}

qreal EntityItem::getChildSize()
{
    return 10;
}

qreal EntityItem::getChildMargin()
{
    return 10;

}

void EntityItem::childPositionChanged()
{
    childUpdated();
}

void EntityItem::childSizeChanged()
{
    childUpdated();
}


/**
 * @brief EntityItem::firstChildAdded
 */
void EntityItem::firstChildAdded(GraphMLItem* child)
{
    if (IS_VECTOR) {
        QString childKind = child->getNodeKind();
        if (childKind.startsWith("Member")) {
            childKind = "Member";
        }
        vectorIconURL = nodeKind + "_" + childKind;
        changeIcon = true;
        emit entityItem_firstChildAdded(getID());
    }
}


/**
 * @brief EntityItem::lastChildRemoved
 */
void EntityItem::lastChildRemoved()
{
    //Update
    if(isExpanded()){
        emit GraphMLItem_SetData(getID(), "isExpanded", false);
    }

    if (IS_VECTOR) {
        vectorIconURL = nodeKind;
        changeIcon = true;
        emit entityItem_lastChildRemoved(getID());
    }
}


qreal EntityItem::getItemMargin() const
{
    return (MARGIN_RATIO * ITEM_SIZE);
}

double EntityItem::getChildItemMargin()
{
    return getItemMargin();
}

double EntityItem::ignoreInsignificantFigures(double model, double current)
{
    double absDifferences = fabs(model-current);
    if(absDifferences > .1){
        return current;
    }
    return model;
}





/**
 * @brief EntityItem::updateSceneRect
 * This gets called everytime there has been a change to the view's sceneRect.
 * @param sceneRect
 */
void EntityItem::sceneRectChanged(QRectF sceneRect)
{
    currentSceneRect = sceneRect;
}

void EntityItem::labelUpdated(QString newLabel)
{
    if(getEntityAdapter()){

        QString currentLabel = getDataValue("label").toString();

        if(currentLabel != newLabel){
            setData("label", newLabel);
        }
    }
}

void EntityItem::setNewLabel(QString newLabel)
{
    if(getEntityAdapter()){
        if(newLabel != ""){
            setData("label", newLabel);
        }else{
            if(!getEntityAdapter()->isDataProtected("label")){
                topLabelInputItem->setEditMode(true);
            }
        }
    }

}


NodeAdapter *EntityItem::getNodeAdapter()
{
    return (NodeAdapter*) getEntityAdapter();
}



QString EntityItem::getNodeLabel()
{
    return nodeLabel;
}


/**
 * @brief EntityItem::setHidden
 * This method is used to prevent this item from being shown
 * when the view aspects are changed. If this item is meant to
 * be hidden no matter the view aspect, this keeps it hidden.
 */
void EntityItem::setHidden(bool h)
{
    hidden = h;
    //setVisible(!h);

    // when the item is no longer hidden, we needs to check if it's in aspect
    // and if its parent is visible before we can display it in the view
    bool parentExpanded = true;
    if (getParentEntityItem()) {
        parentExpanded = getParentEntityItem()->isExpanded();
    }
    setVisibility(!h && parentExpanded);
}


/**
 * @brief EntityItem::resetSize
 * Reset this node item's size to its default size.
 */
void EntityItem::resetSize()
{
    expandedHeight = contractedHeight;
    expandedWidth = contractedWidth;
    emit GraphMLItem_SetData(getID(), "height", contractedHeight);
    emit GraphMLItem_SetData(getID(), "width", contractedWidth);
}


/**
 * @brief EntityItem::isExpanded
 */
bool EntityItem::isExpanded()
{
    return IS_EXPANDED_STATE;
}

bool EntityItem::isContracted()
{
    if(childItems().count() == 0){
        return true;
    }
    return !isExpanded();
}

