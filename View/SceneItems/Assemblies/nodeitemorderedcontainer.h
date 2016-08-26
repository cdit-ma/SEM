#ifndef NODEITEMORDEREDCONTAINER_H
#define NODEITEMORDEREDCONTAINER_H

#include "../containerelementnodeitem.h"

class ContainerNodeItem : public ContainerElementNodeItem
{
    Q_OBJECT
public:
    ContainerNodeItem(NodeViewItem* viewItem, NodeItemNew* parentItem);

    //Pure virtual functions.
    virtual QPointF getElementPosition(ContainerElementNodeItem* child) = 0;
    virtual QPoint getElementIndex(ContainerElementNodeItem* child) = 0;
};

#endif // NODEITEMORDEREDCONTAINER_H
