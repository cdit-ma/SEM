#ifndef EDGEITEMNEW_H
#define EDGEITEMNEW_H

#include "../edgeviewitem.h"

#include "entityitemnew.h"

class NodeItemNew;

class EdgeItemNew : public EntityItemNew{
    Q_OBJECT

public:
    enum KIND{DEFAULT};

    EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew* parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind = DEFAULT);
    ~EdgeItemNew();
    KIND getEdgeItemKind();

    NodeItemNew* getParentItem();
    NodeItemNew* getSourceItem();
    NodeItemNew* getDestinationItem();

public slots:
    void sourceMoved();
    void destinationMoved();
private:
    void updatePosition();
    KIND edge_kind;
    EdgeViewItem* edgeViewItem;
    NodeItemNew* parentItem;
    NodeItemNew* sourceItem;
    NodeItemNew* destinationItem;

    QGraphicsPathItem* bezierCurve1;
    QGraphicsPathItem* bezierCurve2;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    QRectF currentRect() const;

private slots:
    void dataChanged(QString keyName, QVariant data);

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;

    // EntityItemNew interface
public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
};

#endif // EDGEITEMNEW_H
