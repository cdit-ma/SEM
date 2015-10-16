#include "graphmlitem.h"
#include "../../Model/graphml.h"
#include "../Table/attributetablemodel.h"
#include "../nodeview.h"
#include <QObject>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>

GraphMLItem::GraphMLItem(GraphML *attachedGraph, GraphMLItem::GUI_KIND kind)
{
    this->attachedGraph = attachedGraph;
    table = new AttributeTableModel(this);
    renderState = RS_NONE;
    nodeView = 0;
    this->kind = kind;
    IS_DELETING = false;
    IS_SELECTED = false;
    IS_HIGHLIGHTED = false;

    if(attachedGraph){
        this->ID = attachedGraph->getID();
    }else{
        this->ID = -1;
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

void GraphMLItem::detach()
{
    IS_DELETING = true;
    delete table;
    table = 0;
    attachedGraph = 0;
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


bool GraphMLItem::isNodeItem()
{
    return kind == NODE_ITEM;
}

bool GraphMLItem::isEdgeItem()
{
    return kind == NODE_EDGE;
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

void GraphMLItem::setHighlighted(bool isHighlighted)
{
    IS_HIGHLIGHTED = isHighlighted;
    update();
}

bool GraphMLItem::isSelected()
{
    return IS_SELECTED;
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
    if(isHighlighted() != entered){
        emit GraphMLItem_Hovered(getID(), entered);
        //setHighlighted(entered);
    }
}

bool GraphMLItem::canHighlight()
{
    return true;
}

void GraphMLItem::zoomChanged(qreal zoom)
{
    currentZoomFactor = zoom;
}
