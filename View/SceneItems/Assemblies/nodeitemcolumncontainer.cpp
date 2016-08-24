#include "nodeitemcolumncontainer.h"
#include "nodeitemcolumnitem.h"
#include <QDebug>
NodeItemColumnContainer::NodeItemColumnContainer(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemContainer(viewItem, parentItem)
{
    this->columnCount = 1;

    columnSpacing = 10;
    columnWidth = DEFAULT_SIZE * 2.5;
    columnHeight = DEFAULT_SIZE / 2;
    setMinimumHeight(columnHeight);
    setExpandedHeight(columnHeight);
    setBodyPadding(QMarginsF(0,0,0,0));
    setMargin(QMarginsF(0,0,0,0));
    setColumnCount(1);
}

void NodeItemColumnContainer::setColumnCount(int count)
{
    this->columnCount = count;
    setExpandedWidth(bodyRect().left() + ((columnWidth + columnSpacing) * columnCount));
}

QPointF NodeItemColumnContainer::getColumnPosition(int x, int y)
{
    return bodyRect().topLeft() + QPointF((columnWidth + columnSpacing) * x, (columnHeight + columnSpacing) * y);
}

QRectF NodeItemColumnContainer::getColumnRect(int x)
{
    QRectF rect;
    rect.setWidth(columnWidth + columnSpacing);
    rect.setHeight(bodyRect().height());
    rect.moveTopLeft(bodyRect().topLeft() + QPointF(x * rect.width(),0));
    return rect;
}

QPointF NodeItemColumnContainer::getPositionForChild(NodeItemNew *child)
{
    NodeItemColumnItem* nodeItemChild = qobject_cast<NodeItemColumnItem*>(child);

    if(nodeItemChild){
        return getColumnPosition(nodeItemChild->getColumn(), nodeItemChild->getRow()) - QPointF(child->getCenterOffset().x(), 0);
    }else{
        return QPointF();
    }
}

QPoint NodeItemColumnContainer::getIndexForChild(NodeItemColumnItem *child)
{
    QPointF childCenter = child->getCenter() - bodyRect().topLeft();

    QPoint index;
    index.setX(childCenter.x() / (columnWidth + columnSpacing));
    index.setY(childCenter.y() / (columnHeight + columnSpacing));
    return index;
}
