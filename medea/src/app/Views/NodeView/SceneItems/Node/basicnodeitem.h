#ifndef BASICNODEITEM_H
#define BASICNODEITEM_H

#include "nodeitem.h"

class StackNodeItem;
class BasicNodeItem : public NodeItem
{
    Q_OBJECT
    
public:
    BasicNodeItem(NodeViewItem* viewItem, NodeItem* parentItem);

    bool isSortOrdered() const;
    void setSortOrdered(bool ordered);
    
    //Pure virtual functions.
    virtual QPointF getElementPosition(BasicNodeItem* child);

    BasicNodeItem* getParentContainer() const;
    StackNodeItem* getParentStackContainer() const;

    void setPos(const QPointF &pos) override;
    QPointF getNearestGridPoint(QPointF newPos) override;
    QRectF getElementRect(EntityRect rect) const override;
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
private:
    QPointF validateMove(QPointF delta) override;
    
    QRectF connectSourceRect() const;
    QRectF connectTargetRect() const;
    
    QRectF notificationRect() const;

    QRectF headerContent() const;
    QRectF headerContent_Icon() const;
    QRectF headerContent_Icon_Overlay() const;

    QRectF headerContent_Data() const;
    QRectF headerContent_Data_Primary() const;
    QRectF headerContent_Data_Secondary() const;

    QRectF headerContent_Data_Primary_Text() const;
    QRectF headerContent_Data_Primary_Icon() const;
    QRectF headerContent_Data_Secondary_Text() const;
    QRectF headerContent_Data_Secondary_Icon() const;

    BasicNodeItem* parentContainer = nullptr;
    StackNodeItem* parentStackContainer = nullptr;
    
    bool is_sorted_ordered = false;

    QMarginsF header_margins;
};

#endif // BASICNODEITEM_H
