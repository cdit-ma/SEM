#include "nodeitemnew.h"
#include "entityitemnew.h"

#include <QDebug>


NodeItemNew::NodeItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, NodeItemNew::KIND kind):EntityItemNew(viewItem, parentItem, EntityItemNew::NODE)
{
    minimumHeight = 0;
    minimumWidth = 0;
    expandedHeight = 0;
    expandedWidth = 0;
    _isExpanded = false;
    gridVisible = true;
    gridEnabled = true;
    aspect = VA_NONE;

    nodeViewItem = viewItem;
    nodeItemKind = kind;

    addRequiredData("x");
    addRequiredData("y");
    addRequiredData("width");
    addRequiredData("height");
    addRequiredData("isExpanded");

    reloadRequiredData();

    if(parentItem){
        //Lock child in same aspect as parent
        setAspect(parentItem->getAspect());

        parentItem->addChildNode(this);
        setParentItem(parentItem);
    }
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
    if(gridEnabled != enabled){
        gridEnabled = enabled;
        if(gridEnabled){
            updateGridLines();
        }
    }
}

bool NodeItemNew::isGridEnabled() const
{
    return gridEnabled;
}

void NodeItemNew::setGridVisible(bool visible)
{
    if(gridVisible != visible){
        gridVisible = visible;
        update();
    }
}

bool NodeItemNew::isGridVisible() const
{
    return isGridEnabled() && gridVisible;
}

QRectF NodeItemNew::sceneBoundingRect() const
{
    return EntityItemNew::sceneBoundingRect();
}

QRectF NodeItemNew::boundingRect() const
{
    QRectF rect;
    rect.setWidth(margin.left() + margin.right() + getWidth());
    rect.setHeight(margin.top() + margin.bottom() + getHeight());
    return rect;
}

QRectF NodeItemNew::contractedRect() const
{
    return QRectF(getMarginOffset(), QSizeF(getMinimumWidth(), getMinimumHeight()));
}

QRectF NodeItemNew::expandedRect() const
{
    return QRectF(getMarginOffset(), QSizeF(getExpandedWidth(), getExpandedHeight()));
}

QRectF NodeItemNew::currentRect() const
{
    return QRectF(getMarginOffset(), QSizeF(getWidth(), getHeight()));
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
    if(minimumWidth != width){
        minimumWidth = width;
        if(!isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged(getSize());
        }
    }
}

void NodeItemNew::setMinimumHeight(qreal height)
{
    if(minimumHeight != height){
        minimumHeight = height;
        if(!isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged(getSize());
        }
    }
}

void NodeItemNew::setExpandedWidth(qreal width)
{
    if(expandedWidth != width){
        expandedWidth = width;
        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged(getSize());
            updateGridLines();
        }
    }
}

void NodeItemNew::setExpandedHeight(qreal height)
{
    if(expandedHeight != height){
        expandedHeight = height;
        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged(getSize());
            updateGridLines();
        }
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

void NodeItemNew::setMargin(QMarginsF margin)
{
    if(this->margin != margin){
        prepareGeometryChange();
        this->margin = margin;
        update();
    }
}

void NodeItemNew::setPadding(QMarginsF padding)
{
    if(this->padding != padding){
        this->padding = padding;
        updateGridLines();
    }
}

QPointF NodeItemNew::getMarginOffset() const
{
    return QPointF(margin.left(), margin.top());
}

QPointF NodeItemNew::getTopLeftSceneCoordinate() const
{
    return sceneBoundingRect().topLeft();
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

void NodeItemNew::setPos(const QPointF &pos)
{
    if(this->pos() != pos){
        QGraphicsObject::setPos(pos);
        updateGridLines();
        emit positionChanged();
    }
}

void NodeItemNew::setAspect(VIEW_ASPECT aspect)
{
    this->aspect = aspect;
}

VIEW_ASPECT NodeItemNew::getAspect()
{
    return aspect;
}

QMarginsF NodeItemNew::getMargin() const
{
    return margin;
}

QMarginsF NodeItemNew::getPadding() const
{
    return padding;
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

int NodeItemNew::getGridSize() const
{
    return 10;
}

void NodeItemNew::updateGridLines()
{
    //TODO
    if(isGridEnabled()){
        QRectF grid = gridRect();
        int gridSize = getGridSize();
        int majorGridCount = 5;
        gridLines_Major_Horizontal.clear();
        gridLines_Minor_Horizontal.clear();
        gridLines_Major_Vertical.clear();
        gridLines_Minor_Vertical.clear();

        QPointF topLeftOffset = getTopLeftSceneCoordinate();
        QPointF gridScenePos =  topLeftOffset + gridRect().topLeft();

        int gridX = ceil(gridScenePos.x() / gridSize);
        int gridY = ceil(gridScenePos.y() / gridSize);

        //Calculate the coordinate for the next grid line (In Scene coordinates)
        qreal firstGridX = gridX * gridSize;
        qreal firstGridY = gridY * gridSize;

        //Translate back into item coordinates
        qreal offsetX = firstGridX - topLeftOffset.x();
        qreal offsetY = firstGridY - topLeftOffset.y();

        int majorCountX = abs(gridX) % majorGridCount;
        int majorCountY = abs(gridY) % majorGridCount;

        if(gridX > 0 && majorCountX > 0){
            majorCountX = majorGridCount - majorCountX;
        }
        if(gridY > 0 && majorCountY > 0){
            majorCountY = majorGridCount - majorCountY;
        }

        //int majorCountY = gridY % majorGridCount;

        qCritical() <<"Grid X: " << gridX << " X: " << majorCountX;
        for(qreal x = offsetX; x <= grid.right(); x += gridSize){
            QLineF line(x, grid.top(), x, grid.bottom());

            if(majorCountX == 0){
                gridLines_Major_Horizontal.append(line);
                majorCountX = majorGridCount;
            }else{
                gridLines_Minor_Horizontal.append(line);
            }
            majorCountX --;
        }

        for(qreal y = offsetY; y <= grid.bottom(); y += gridSize){
            QLineF line(grid.left(), y, grid.right(), y);
            if(majorCountY == 0){
                gridLines_Major_Horizontal.append(line);
                majorCountY = majorGridCount;
            }else{
                gridLines_Minor_Horizontal.append(line);
            }
            majorCountY --;
        }
    }
}

void NodeItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isGridVisible()){
        painter->setClipRect(gridRect());
        //Paint the grid lines.


        QPen linePen;
        linePen.setColor(Qt::gray);
        linePen.setStyle(Qt::DotLine);
        linePen.setWidthF(.5);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(linePen);

        painter->drawLines(gridLines_Minor_Horizontal);
        painter->drawLines(gridLines_Minor_Vertical);

        linePen.setWidthF(1);
        linePen.setStyle(Qt::SolidLine);
        painter->setPen(linePen);
        painter->drawLines(gridLines_Major_Horizontal);
        painter->drawLines(gridLines_Major_Vertical);
    }
}


