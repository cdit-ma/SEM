#include "nodeitem.h"



NodeItemNew::NodeItemNew(NodeViewItem *viewItem, EntityItem *parentItem, NodeItemNew::KIND kind):EntityItemNew(viewItem, parentItem, EntityItemNew::NODE)
{
    nodeViewItem = viewItem;
    nodeItemKind = kind;

    addRequiredData("x");
    addRequiredData("y");
    addRequiredData("width");
    addRequiredData("height");
    addRequiredData("isExpanded");
}

NodeItemNew *NodeItemNew::getParentNodeItem()
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
    return childNodes;
}

QList<EntityItemNew *> NodeItemNew::getChildEntities()
{
    QList<EntityItemNew*> children = childNodes.values();
    children.append(childEdges.values());
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
    return childEdges;
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
    return proxyChildEdges;
}

void NodeItemNew::setGridEnabled(bool enabled)
{
    gridEnabled = enabled;
    if(gridEnabled){
        updateGridLines();
    }
}

bool NodeItemNew::isGridEnabled()
{
    return gridEnabled;
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
    minimumWidth = width;
}

void NodeItemNew::setMinimumHeight(qreal height)
{
    minimumHeight = height;
}

void NodeItemNew::setExpandedWidth(qreal width)
{
    expandedWidth = width;
}

void NodeItemNew::setExpandedHeight(qreal height)
{
    expandedHeight = height;
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

qreal NodeItemNew::getWidth() const
{
    if(_isExpanded){
        return getMinimumWidth();
    }else{
        return getExpandedWidth();
    }
}

qreal NodeItemNew::getHeight() const
{
    if(_isExpanded){
        return getMinimumHeight();
    }else{
        return getExpandedHeight();
    }
}

QPointF NodeItemNew::getCenter() const
{
    return pos() + contractedRect().center();
}

void NodeItemNew::setCenter(QPointF center)
{
    center -= contractedRect().center();
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

        emit sizeChanged(getSize());
    }
}

void NodeItemNew::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "x" || keyName == "y"){
        qreal realData = data.toReal();
        QPointF oldCenter = getCenter();
        PointF newCenter = oldCenter;

        if(keyName == "x"){
            currentCenter.setX(realData);
        }else if(keyName == "y"){
            currentCenter.setY(realData);
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


