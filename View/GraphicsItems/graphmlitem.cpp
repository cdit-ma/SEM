#include "graphmlitem.h"
#include "../../Model/graphml.h"
#include "../Table/attributetablemodel.h"
#include "../nodeview.h"
#include <QObject>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>
#include "nodeitem.h"
#include "entityitem.h"
#include <QPen>

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

#define THEME_LIGHT 0
#define THEME_DARK 1

#define THEME_DARK_NEUTRAL 10
#define THEME_DARK_COLOURED 11

GraphMLItem::GraphMLItem(EntityAdapter *graph, GraphMLItem* parent, GraphMLItem::GUI_KIND kind)
{
    graph->addListener(this);
    connect(graph, SIGNAL(dataChanged(QString,QVariant)), this, SLOT(dataChanged(QString,QVariant)));
    connect(graph, SIGNAL(dataAdded(QString,QVariant)), this, SLOT(dataChanged(QString,QVariant)));
    connect(graph, SIGNAL(dataRemoved(QString)), this, SLOT(dataRemoved(QString)));
    parentItem = 0;
    attachedGraph = graph;

    nodeView = 0;

    //Cache on Graphics card! May Artifact.
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);



    table = new AttributeTableModel(this);
    renderState = RS_NONE;
    this->kind = kind;
    IS_DELETING = false;
    IS_SELECTED = false;
    IS_HOVERED = false;
    IS_HIGHLIGHTED = false;
    IN_SUBVIEW = false;
    errorType = ET_OKAY;

    ID = -1;

    if(attachedGraph){
        ID = attachedGraph->getID();
        if(attachedGraph->isNodeAdapter()){
            nodeKind = attachedGraph->getDataValue("kind").toString();
        }
    }

    setFlag(QGraphicsItem::ItemIsSelectable, false);

    setAcceptHoverEvents(true);

    setupPens();

    if(parent){
        setParent(parent);
    }
}

GraphMLItem::RENDER_STATE GraphMLItem::getRenderState() const
{
    return renderState;
}

VIEW_STATE GraphMLItem::getViewState() const
{
    if(nodeView){
        return nodeView->getViewState();
    }
    return VS_NONE;
}

bool GraphMLItem::intersectsRectangle(QRectF sceneRect)
{
    return sceneRect.contains(sceneBoundingRect());
}

void GraphMLItem::listenForData(QString keyName)
{
    if(!connectedDataKeys.contains(keyName)){
        connectedDataKeys.append(keyName);
    }
}

void GraphMLItem::setRenderState(GraphMLItem::RENDER_STATE renderState)
{
    if(this->renderState != renderState){
        this->renderState = renderState;
    }
}

/**
 * @brief GraphMLItem::firstChildAdded
 * @param item
 */
void GraphMLItem::firstChildAdded(GraphMLItem*)
{
    // Do nothing
}

void GraphMLItem::lastChildRemoved()
{
    //Do nothing
}

void GraphMLItem::addChild(GraphMLItem *item)
{
    int ID = item->getID();
    children[ID] = item;
    if (children.count() == 1) {
        firstChildAdded(item);
    }
}

void GraphMLItem::removeChild(int ID)
{
    children.remove(ID);
    if(children.isEmpty()){
        lastChildRemoved();
    }
}

bool GraphMLItem::hasChildren()
{
    return !children.isEmpty();
}

GraphMLItem *GraphMLItem::getParent()
{
    return parentItem;
}

void GraphMLItem::setParent(GraphMLItem *item)
{
    parentItem = item;
    if(item){
        item->addChild(this);
        setParentItem(item);
        setNodeView(item->getNodeView());
    }
}

QList<GraphMLItem *> GraphMLItem::getChildren()
{
    return children.values();
}


void GraphMLItem::updateFromData()
{
    foreach(QString keyName, connectedDataKeys){
        QVariant dataValue = getEntityAdapter()->getDataValue(keyName);
        if(!dataValue.isNull()){
            dataChanged(keyName, dataValue);
        }
    }
}

void GraphMLItem::setData(QString keyName, QVariant value)
{
    if(!getEntityAdapter()->isDataProtected(keyName)){
        emit GraphMLItem_SetData(getID(), keyName, value);
    }
}

bool GraphMLItem::isDataDifferent(QString keyName, QVariant currentValue)
{
    QVariant modelValue = getDataValue(keyName);
    return modelValue != currentValue;
}

void GraphMLItem::detach()
{
    IS_DELETING = true;
    delete table;
    table = 0;
    attachedGraph = 0;
}

void GraphMLItem::setInSubView(bool inSubview)
{
    IN_SUBVIEW = inSubview;
}

bool GraphMLItem::isDeleting()
{
    return IS_DELETING;
}

void GraphMLItem::updateData(QString keyName)
{
    QVariant data = getDataValue(keyName);
    dataChanged(keyName, data);
}

GraphMLItem::~GraphMLItem()
{
    if(table){
        table->deleteLater();
        table = 0;
    }
    getEntityAdapter()->removeListener(this);
}

QRectF GraphMLItem::sceneBoundingRect() const
{
    return QGraphicsObject::sceneBoundingRect();
}

QRectF GraphMLItem::boundingRect() const
{
    return QRect();
}

QRectF GraphMLItem::childrenBoundingRect()
{
    QRectF rect;

    foreach(GraphMLItem* child, getChildren()){
        rect = rect.united(child->translatedBoundingRect());
    }
    return rect;
}

QRectF GraphMLItem::translatedBoundingRect()
{
    QRectF rect = boundingRect();
    rect.translate(pos());
    return rect;
}

QString GraphMLItem::getNodeKind()
{
    return nodeKind;
}

bool GraphMLItem::hasGraphMLKey(QString keyName)
{
    if(getEntityAdapter()){
        return getEntityAdapter()->hasData(keyName);
    }
    return false;
}

EntityAdapter *GraphMLItem::getEntityAdapter()
{
    return attachedGraph;
}

AttributeTableModel *GraphMLItem::getAttributeTable()
{
    return table;
}

void GraphMLItem::setNodeView(NodeView *view)
{
    nodeView = view;
    if(nodeView){
        setInSubView(nodeView->isSubView());
        zoomChanged(nodeView->getCurrentZoom());
    }
}

NodeView *GraphMLItem::getNodeView()
{
    return nodeView;
}

QVariant GraphMLItem::getDataValue(QString key)
{
    if(getEntityAdapter()){
        return getEntityAdapter()->getDataValue(key);
    }
    return QVariant();
}

bool GraphMLItem::inMainView()
{
    return !IN_SUBVIEW;
}

bool GraphMLItem::inSubView()
{
    return IN_SUBVIEW;
}



bool GraphMLItem::isEntityItem()
{
    return kind == ENTITY_ITEM;
}

bool GraphMLItem::isAspectItem()
{
    return kind == ASPECT_ITEM;
}

bool GraphMLItem::isNodeItem()
{
    return kind == ENTITY_ITEM || kind == ASPECT_ITEM;
}

bool GraphMLItem::isEdgeItem()
{
    return kind == NODE_EDGE;
}

bool GraphMLItem::isModelItem()
{
    return kind == MODEL_ITEM;
}

bool GraphMLItem::isDataEditable(QString keyName)
{
    if(attachedGraph){
        return !attachedGraph->isDataProtected(keyName);
    }
    return false;
}

QPen GraphMLItem::getCurrentPen()
{
    return currentPen;
}

int GraphMLItem::getID()
{
    return this->ID;
}

qreal GraphMLItem::getZoomFactor()
{
    return currentZoomFactor;
}

void GraphMLItem::setSelected(bool selected)
{
    if(IS_SELECTED != selected){
        IS_SELECTED = selected;
        updateCurrentPen();
        update();
    }
}

void GraphMLItem::dataRemoved(QString keyName)
{
    dataChanged(keyName, QVariant());
}

void GraphMLItem::setHovered(bool isHovered)
{
    if(IS_HOVERED != isHovered){
        IS_HOVERED = isHovered;
        if(!IS_HOVERED){
            unsetCursor();
        }
        updateCurrentPen();
        update();
    }
}

void GraphMLItem::setHighlighted(bool isHighlighted)
{
    if(IS_HIGHLIGHTED != isHighlighted){
        IS_HIGHLIGHTED = isHighlighted;
        update();
    }
}

bool GraphMLItem::isSelected()
{
    return IS_SELECTED;
}

bool GraphMLItem::isHovered()
{
    return IS_HOVERED;
}

bool GraphMLItem::isHighlighted()
{
    return IS_HIGHLIGHTED;
}

void GraphMLItem::handleSelection(bool setSelected, bool controlDown)
{
    if(isSelected() && controlDown){
        //DeSelect on control click.
        setSelected = false;
    }

    bool hasChanged = false;
    if(isSelected() != setSelected){
        if(setSelected && !controlDown){
            emit GraphMLItem_ClearSelection();
            hasChanged = true;
        }
        if(getNodeView() && setSelected){
            getNodeView()->setStateSelected();
            emit GraphMLItem_AppendSelected(this);
            hasChanged = true;
        }else{
            emit GraphMLItem_RemoveSelected(this);
            hasChanged = true;
        }
    }
    if(hasChanged){
        //Emit a signal which is connected to the docks to update stuffs.
        emit GraphMLItem_SelectionChanged();
    }
}

void GraphMLItem::handleHover(bool entered)
{
    if(canHover() && (IS_HOVERED != entered)){
        emit GraphMLItem_Hovered(getID(), entered);
    }
}

QRectF GraphMLItem::adjustRectForPen(QRectF rect, QPen pen)
{
    qreal penWidth = pen.widthF() / 2;
    rect.adjust(penWidth, penWidth, -penWidth, -penWidth);
    return rect;
}


bool GraphMLItem::canHover()
{
    return true;
}

void GraphMLItem::zoomChanged(qreal zoom)
{
    currentZoomFactor = zoom;
    updateCurrentPen(true);
}

void GraphMLItem::setupPens()
{
    defaultPen.setWidth(1);
    defaultPen.setColor(Qt::black);
    selectedPen.setColor(Qt::blue);
    selectedPen.setWidth(1);
    currentPen = defaultPen;
}

void GraphMLItem::updateCurrentPen(bool zoomChanged)
{
    if(zoomChanged){
        //Update Pen Width!
        selectedPenWidth = qMax(SELECTED_LINE_WIDTH / currentZoomFactor, 1.0);
        selectedPen.setWidthF(selectedPenWidth);
    }

    bool useDefault = !isSelected();

    if(useDefault){
        currentPen = defaultPen;
    }else{
        currentPen = selectedPen;
    }

    if(isHovered()){
        if(useDefault){
            currentPen.setWidthF(selectedPenWidth);
            currentPen.setColor(QColor(130,130,130));
        }else{
            // remove hover highlight for aspect items altogether
            if (!isAspectItem()) {
                currentPen.setColor(currentPen.color().lighter(130));
            }
        }
    }
}

void GraphMLItem::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    handleHover(true);
}

void GraphMLItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    handleHover(false);
    unsetCursor();
}

void GraphMLItem::hoverMoveEvent(QGraphicsSceneHoverEvent *)
{
    handleHover(true);
}
