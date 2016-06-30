#ifndef NODEITEMNEW_H
#define NODEITEMNEW_H

#include "entityitemnew.h"
#include "../nodeviewitem.h"
#include "edgeitemnew.h"
#include <QRectF>
#include <QPointF>
class NodeItemNew: public EntityItemNew
{
    Q_OBJECT
public:
    enum KIND{DEFAULT_ITEM, MODEL_ITEM, ASPECT_ITEM, PARAMETER_ITEM, QOS_ITEM};
    NodeItemNew(NodeViewItem *viewItem, NodeItemNew* parentItem, KIND kind);
    ~NodeItemNew();

    NodeItemNew* getParentNodeItem() const;
    KIND getNodeItemKind();

    void addChildNode(NodeItemNew* nodeItem);
    void removeChildNode(int ID);

    QList<NodeItemNew*> getChildNodes();
    QList<EntityItemNew*> getChildEntities() const;

    void addChildEdge(EdgeItemNew* edgeItem);
    void removeChildEdge(int ID);
    QList<EdgeItemNew*> getChildEdges();

    void addProxyEdge(EdgeItemNew* edgeItem);
    void removeProxyEdge(int ID);
    QList<EdgeItemNew*> getProxyEdges();

    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;

    virtual QRectF sceneBoundingRect() const;


    //RECTS
    virtual QRectF boundingRect() const;
    virtual QRectF contractedRect() const;
    virtual QRectF expandedRect() const;
    virtual QRectF gridRect() const;
    QRectF childrenRect() const;

    QSizeF getSize() const;

    void setMinimumWidth(qreal width);
    void setMinimumHeight(qreal height);

    //Size/Position Functions
    void setExpandedWidth(qreal width);
    void setExpandedHeight(qreal height);
    void setMarginSize(qreal size);
    qreal getExpandedWidth() const;
    qreal getExpandedHeight() const;
    qreal getMinimumWidth() const;
    qreal getMinimumHeight() const;

    qreal getMarginSize() const;
    QPointF getMarginOffset() const;
    qreal getWidth() const;
    qreal getHeight() const;

    QPointF getCenter() const;
    void setCenter(QPointF center);

    bool isExpanded() const;

signals:
    //Request changes
    void req_adjustPos(QPointF delta);
    void req_adjustPosFinished();

    void req_adjustSize(NodeViewItem* item, QSizeF delta);
    void req_adjustSizeFinished();

    //Inform of Changes
    void gotChildNodes(bool);
    void gotChildProxyEdges(bool);

    void posChanged(QPointF topLeft);
    void sizeChanged(QSizeF newSize);

public slots:
    void setExpanded(bool);
    virtual void dataChanged(QString keyName, QVariant data);
private:
    void updateGridLines();
    NodeViewItem* nodeViewItem;
    KIND nodeItemKind;


    qreal minimumWidth;
    qreal minimumHeight;
    qreal expandedWidth;
    qreal expandedHeight;

    qreal marginSize;
    bool _isExpanded;

    bool gridEnabled;

    QHash<int, NodeItemNew*> childNodes;
    QHash<int, EdgeItemNew*> childEdges;
    QHash<int, EdgeItemNew*> proxyChildEdges;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    virtual QRectF getElementRect(ELEMENT_RECT rect) = 0;
};
#endif
