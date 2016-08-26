#ifndef CONTAINERELEMENTNODEITEM_H
#define CONTAINERELEMENTNODEITEM_H

#include "nodeitemnew.h"

class ContainerNodeItem;

class ContainerElementNodeItem : public NodeItemNew
{
    Q_OBJECT
public:
    ContainerElementNodeItem(NodeViewItem* viewItem, NodeItemNew* parentItem);

    ContainerNodeItem* getContainer() const;

    QPoint getIndexPosition() const;
    void setIndexPosition(QPoint point);
    void setPos(const QPointF &pos);
private:
    ContainerNodeItem* container;
    QPoint indexPosition;
};

#endif // CONTAINERELEMENTNODEITEM_H
