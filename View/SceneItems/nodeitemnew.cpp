#include "nodeitemnew.h"
#include "entityitemnew.h"

#include <QDebug>


NodeItemNew::NodeItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, NodeItemNew::KIND kind):EntityItemNew(viewItem, parentItem, EntityItemNew::NODE)
{
    minimumHeight = 0;
    minimumWidth = 0;
    expandedHeight = 0;
    expandedWidth = 0;
    marginSize = 0;
    _isExpanded = false;
    qCritical() << "NodeItemNew()";
    nodeViewItem = viewItem;
    nodeItemKind = kind;

    addRequiredData("x");
    addRequiredData("y");
    addRequiredData("width");
    addRequiredData("height");
    addRequiredData("isExpanded");

    reloadRequiredData();


    if(parentItem){
        parentItem->addChildNode(this);
        setParentItem(parentItem);
    }


    qCritical() << "NodeItemNew()";
}

NodeItemNew::~NodeItemNew()
{

}

NodeItemNew *NodeItemNew::getParentNodeItem() const
{
    EntityItemNew* parent = getParent();
    if(parent && parent->isNodeItem()){
        return (NodeItemNew*) parent;
    }
    return 0;
}

NodeItemNew::KIND NodeItemNew::getNodeItemKind()
{
    return nodeItemKind;
}

void NodeItemNew::addChildNode(NodeItemNew *nodeItem)
{
    int ID = nodeItem->getID();
    //If we have added a child, and there is only one. emit a signal
    if(!childNodes.contains(ID)){
        childNodes[ID] = nodeItem;
        if(childNodes.count() == 1){
            emit gotChildNodes(true);
        }
    }
}

void NodeItemNew::removeChildNode(int ID)
{
    //If we have removed a child, and there is no children left. emit a signal
    if(childNodes.remove(ID) > 0){
        if(childNodes.count() == 0){
            emit gotChildNodes(false);
        }
    }
}

QList<NodeItemNew *> NodeItemNew::getChildNodes()
{
    return childNodes.values();
}

QList<EntityItemNew *> NodeItemNew::getChildEntities() const
{
    QList<EntityItemNew*> children;
    foreach(NodeItemNew* child, childNodes.values()){
        children.append(child);
    }
    foreach(EdgeItemNew* child, childEdges.values()){
        children.append(child);
    }
    return children;
}

void NodeItemNew::addChildEdge(EdgeItemNew *edgeItem)
{
    int ID = edgeItem->getID();
    if(!childEdges.contains(ID)){
        childEdges[ID] = edgeItem;
    }
}

void NodeItemNew::removeChildEdge(int ID)
{
    childEdges.remove(ID);
}

QList<EdgeItemNew *> NodeItemNew::getChildEdges()
{
    return childEdges.values();
}

void NodeItemNew::addProxyEdge(EdgeItemNew *edgeItem)
{
    int ID = edgeItem->getID();
    if(!proxyChildEdges.contains(ID)){
        proxyChildEdges[ID] = edgeItem;

        if(proxyChildEdges.count() == 1){
            emit gotChildProxyEdges(true);
        }
    }
}

void NodeItemNew::removeProxyEdge(int ID)
{
    if(proxyChildEdges.remove(ID) > 0){
        if(proxyChildEdges.count() == 0){
            emit gotChildProxyEdges(false);
        }
    }
}

QList<EdgeItemNew *> NodeItemNew::getProxyEdges()
{
    return proxyChildEdges.values();
}

void NodeItemNew::setGridEnabled(bool enabled)
{
    gridEnabled = enabled;
    if(gridEnabled){
        updateGridLines();
    }
}

bool NodeItemNew::isGridEnabled() const
{
    return gridEnabled;
}

QRectF NodeItemNew::sceneBoundingRect() const
{
    return EntityItemNew::sceneBoundingRect();
}

QRectF NodeItemNew::boundingRect() const
{
    qreal margin = 2 * getMarginSize();

    QRectF rect;
    rect.setWidth(margin + getWidth());
    rect.setHeight(margin + getHeight());
    return rect;
}

QRectF NodeItemNew::contractedRect() const
{
    qreal margin = getMarginSize();
    QRectF rect;
    rect.setLeft(margin);
    rect.setTop(margin);
    rect.setWidth(getMinimumWidth());
    rect.setHeight(getMinimumHeight());
    return rect;
}

QRectF NodeItemNew::expandedRect() const
{
    qreal margin = getMarginSize();
    QRectF rect;
    rect.setLeft(margin);
    rect.setTop(margin);
    rect.setWidth(getExpandedWidth());
    rect.setHeight(getExpandedHeight());
    return rect;
}

QRectF NodeItemNew::gridRect() const
{
    return QRectF(10,10,10,10);
}


QRectF NodeItemNew::childrenRect() const
{
    QRectF rect;
    foreach(EntityItemNew* child, getChildEntities()){
        rect = rect.united(child->translatedBoundingRect());
    }
    return rect;
}

QSizeF NodeItemNew::getSize() const
{
    QSizeF size;
    size.setWidth(getWidth());
    size.setHeight(getHeight());
    return size;
}

void NodeItemNew::setMinimumWidth(qreal width)
{
    prepareGeometryChange();
    minimumWidth = width;
    update();
}

void NodeItemNew::setMinimumHeight(qreal height)
{
    prepareGeometryChange();
    minimumHeight = height;
    update();
}

void NodeItemNew::setExpandedWidth(qreal width)
{
    prepareGeometryChange();
    expandedWidth = width;
    update();
}

void NodeItemNew::setExpandedHeight(qreal height)
{
    prepareGeometryChange();
    expandedHeight = height;
    update();
}

void NodeItemNew::setMarginSize(qreal size)
{
    if(marginSize != size){
        prepareGeometryChange();
        marginSize = size;
        update();
    }
}

qreal NodeItemNew::getExpandedWidth() const
{
    return expandedWidth;
}

qreal NodeItemNew::getExpandedHeight() const
{
    return expandedHeight;
}

qreal NodeItemNew::getMinimumWidth() const
{
    return minimumWidth;
}

qreal NodeItemNew::getMinimumHeight() const
{
    return minimumHeight;
}

qreal NodeItemNew::getMarginSize() const
{
    return marginSize;
}

QPointF NodeItemNew::getMarginOffset() const
{
    return QPointF(marginSize, marginSize);
}

qreal NodeItemNew::getWidth() const
{
    if(_isExpanded){
        return getExpandedWidth();
    }else{
        return getMinimumWidth();
    }
}

qreal NodeItemNew::getHeight() const
{
    if(_isExpanded){
        return getExpandedHeight();
    }else{
        return getMinimumHeight();
    }
}

QPointF NodeItemNew::getCenter() const
{
    QPointF center = pos() + contractedRect().center();

    NodeItemNew* parent = getParentNodeItem();

    if(parent){
        center -= parent->getMarginOffset();
    }
    return center;
}

void NodeItemNew::setCenter(QPointF center)
{
    center -= contractedRect().center();

    NodeItemNew* parent = getParentNodeItem();

    if(parent){
        center += parent->getMarginOffset();
    }

    setPos(center);
}

bool NodeItemNew::isExpanded() const
{
    return _isExpanded;
}

void NodeItemNew::setExpanded(bool expand)
{
    if(_isExpanded != expand){
        prepareGeometryChange();
        _isExpanded = expand;

        //Hide/Show Children
        foreach(NodeItemNew* child, getChildNodes()){
            child->setVisible(_isExpanded);
        }

        update();
        emit sizeChanged(getSize());
    }
}

void NodeItemNew::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "x" || keyName == "y"){
        qreal realData = data.toReal();
        QPointF oldCenter = getCenter();
        QPointF newCenter = oldCenter;

        if(keyName == "x"){
            newCenter.setX(realData);
        }else if(keyName == "y"){
            newCenter.setY(realData);
        }

        if(newCenter != oldCenter){
            setCenter(newCenter);
        }
    }else if(keyName == "width" || keyName == "height"){
        qreal realData = data.toReal();
        if(keyName == "width"){
            setExpandedWidth(realData);
        }else if(keyName == "height"){
            setExpandedHeight(realData);
        }
    }else if(keyName == "isExpanded"){
        bool boolData = data.toBool();
        setExpanded(boolData);
    }
}

void NodeItemNew::updateGridLines()
{
    //TODO

}

void NodeItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
}


