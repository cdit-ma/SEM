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

GraphMLItem::GraphMLItem(GraphML *attachedGraph, GraphMLItem::GUI_KIND kind)
{
    //Cache on Graphics card! May Artifact.
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    parentItem = 0;
    this->attachedGraph = attachedGraph;
    table = new AttributeTableModel(this);
    renderState = RS_NONE;
    nodeView = 0;
    this->kind = kind;
    IS_DELETING = false;
    IS_SELECTED = false;
    IS_HOVERED = false;
    IS_HIGHLIGHTED = false;
    IN_SUBVIEW = false;

    ID = -1;

    if(attachedGraph && attachedGraph->isNode()){
        ID = attachedGraph->getID();
        nodeKind = attachedGraph->getDataValue("kind");
    }

    setFlag(QGraphicsItem::ItemIsSelectable, false);

    setAcceptHoverEvents(true);


}

GraphMLItem::RENDER_STATE GraphMLItem::getRenderState() const
{
    return renderState;
}

bool GraphMLItem::intersectsRectangle(QRectF sceneRect)
{
    return sceneRect.contains(sceneBoundingRect());
}

void GraphMLItem::setRenderState(GraphMLItem::RENDER_STATE renderState)
{
    if(this->renderState != renderState){
        this->renderState = renderState;
    }
}

void GraphMLItem::addChild(GraphMLItem *item)
{
    int ID = item->getID();
    children[ID] = item;
}

void GraphMLItem::removeChild(int ID)
{
    children.remove(ID);
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
        this->setParentItem(item);
    }
}

QList<GraphMLItem *> GraphMLItem::getChildren()
{
    return children.values();
}

void GraphMLItem::connectToGraphMLData(QString keyName)
{
    if(getGraphML()){
        connectToGraphMLData(getGraphML()->getData(keyName));
    }
}

void GraphMLItem::connectToGraphMLData(GraphMLData *data)
{
    if(data){
        int ID = data->getID();
        if(!connectedDataIDs.contains(ID)){
            connect(data, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
            connectedDataIDs.append(ID);
        }
    }
}

void GraphMLItem::updateFromGraphMLData()
{
    for(int i = 0; i < connectedDataIDs.size(); i++){
        int ID = connectedDataIDs.at(i);
        GraphMLData* data = getGraphML()->getData(ID);
        if(data){
            graphMLDataChanged(data);
        }
    }
}

void GraphMLItem::setGraphMLData(QString keyName, QString value)
{
    GraphMLData* data = getGraphML()->getData(keyName);
    if(data){
        data->setValue(value);
    }
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

GraphMLItem::~GraphMLItem()
{
    if(table){
        delete table;
        table = 0;
    }
    if(getNodeView()){
        if(!getNodeView()->isTerminating()){
            if(parentItem){
                parentItem->removeChild(getID());
            }
        }
    }
}

QString GraphMLItem::getNodeKind()
{
    return nodeKind;
}

QString GraphMLItem::getGraphMLDataValue(QString key)
{
    if(getGraphML() && !getGraphML()->isDeleting()){
        GraphMLData* data = getGraphML()->getData(key);
        if(!data->isDeleting() && data->getParent() == getGraphML()){
            return data->getValue();
        }
    }
    return QString();
}

GraphML *GraphMLItem::getGraphML()
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
}

NodeView *GraphMLItem::getNodeView()
{
    return nodeView;
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
        return attachedGraph->getData(keyName) && (!attachedGraph->getData(keyName)->isProtected());
    }
    return false;
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
    IS_SELECTED = selected;
    update();
}

void GraphMLItem::setHovered(bool isHovered)
{
    IS_HOVERED = isHovered;
    update();
}

void GraphMLItem::setHighlighted(bool isHighlighted)
{
    IS_HIGHLIGHTED = isHighlighted;
    update();
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

    if(isSelected() != setSelected){
        if(setSelected && !controlDown){
            emit GraphMLItem_ClearSelection();
        }
        if(setSelected){
            getNodeView()->setStateSelected();
            emit GraphMLItem_AppendSelected(this);
        }else{
            emit GraphMLItem_RemoveSelected(this);
        }
    }
}

void GraphMLItem::handleHighlight(bool entered)
{
    if(isHovered() != entered){
        emit GraphMLItem_Hovered(getID(), entered);
        //setHighlighted(entered);
    }
}


bool GraphMLItem::canHover()
{
    return true;
}

void GraphMLItem::zoomChanged(qreal zoom)
{
    currentZoomFactor = zoom;
}
