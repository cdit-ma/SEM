#ifndef COMPACTNODEITEM_H
#define COMPACTNODEITEM_H

#include "basicnodeitem.h"

class CompactNodeItem : public BasicNodeItem
{
    Q_OBJECT
public:
    CompactNodeItem(NodeViewItem* viewItem, NodeItem* parentItem);
    
    QRectF getElementRect(EntityRect rect) const override;

protected:
    QRectF childrenRect() const override;
    
private:
    bool isExpandEnabled() override;

    // NOTE: This is a non-virtual function - NodeItem's function is used within
    QRectF headerRect() const;
    
    QRectF primaryRow() const;
    QRectF secondaryRow() const;
    QRectF tertiaryRow() const;

    QRectF textRect_Primary() const;
    QRectF textRect_Secondary() const;
    QRectF textRect_Tertiary() const;

    QRectF iconRect_Primary() const;
    QRectF iconRect_Secondary() const;
    QRectF iconRect_Tertiary() const;
};

#endif // COMPACTNODEITEM_H
