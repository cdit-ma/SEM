#include "stacknodeitem.h"

#include <QDebug>
#include <cmath>

StackNodeItem::StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation):
    BasicNodeItem(viewItem, parentItem)
{
    setSortOrdered(true);
    setResizeEnabled(false);
    setGridEnabled(false);

    removeRequiredData("width");
    removeRequiredData("height");
    reloadRequiredData();
    this->orientation = orientation;
}

QPointF StackNodeItem::getStemAnchorPoint() const
{
    //QPointF offset;
    return gridRect().topLeft();
}

QPointF StackNodeItem::getElementPosition(BasicNodeItem *child)
{
    int childPos = child->getSortOrder();
    int row = child->getSortOrderRow();
    int gridSize = getGridSize();

    QPointF itemOffset = gridRect().topLeft();

    //offset to the nearest grid
    qreal xMod = fmod(itemOffset.x(), gridSize);
    qreal yMod = fmod(itemOffset.y(), gridSize);

    if(xMod > 0){
        itemOffset.rx() += gridSize - xMod;
    }

    if(yMod > 0){
        itemOffset.ry() += gridSize - yMod;
    }

    QMap<int, QSizeF> row_sizes;

    if(orientation == Qt::Horizontal){
        for(auto c : getChildNodes()){
            auto c_row = c->getSortOrderRow();
            auto c_size = c->currentRect();

            if(c_row == row && c->getSortOrder() < childPos){
                row_sizes[c_row].rwidth() += c_size.width() + 4 * gridSize;
            }
            if(row_sizes[c_row].height() < c_size.height()){
                row_sizes[c_row].rheight() = c_size.height();
            }
        }

        for(int i = 0; i <= row; i++){
            if(row_sizes.contains(i)){
                if(i < row){
                    itemOffset.ry() += row_sizes[i].height() + 2 * gridSize;;
                }else{
                    itemOffset.rx() += row_sizes[i].width();
                }
            }
        }
    }else{
        for(auto c : getChildNodes()){
            auto c_row = c->getSortOrderRow();
            auto c_size = c->currentRect();

            if(c_row == row && c->getSortOrder() < childPos){
                row_sizes[c_row].rheight() += c_size.height() + gridSize;
            }
            if(row_sizes[c_row].width() < c_size.width()){
                row_sizes[c_row].rwidth() = c_size.width();
            }
        }

        for(int i = 0; i <= row; i++){
            if(row_sizes.contains(i)){
                if(i < row){
                    itemOffset.rx() += row_sizes[i].width() + gridSize;
                }else{
                    itemOffset.ry() += row_sizes[i].height();
                }
            }
        }
    }
    return itemOffset;
}

void StackNodeItem::childPosChanged()
{
    QList<NodeItem*> children = getChildNodes();
    foreach(NodeItem* child, children){
        if(!child->isMoving()){
            child->setPos(QPointF());
        }
    }
    NodeItem::childPosChanged();
    update();
}
