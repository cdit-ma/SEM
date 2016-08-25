#ifndef NODEITEMORDEREDCONTAINER_H
#define NODEITEMORDEREDCONTAINER_H

#include "../nodeitemcontainer.h"
class NodeItemColumnItem;
class NodeItemOrderedContainer : public NodeItemContainer
{
    Q_OBJECT
public:
    NodeItemOrderedContainer(NodeViewItem* viewItem, NodeItemNew* parentItem);

    NodeItemOrderedContainer* getContainerParent() const;
    //Pure virtual functions.
    virtual QPointF getFixedPositionForChild(NodeItemOrderedContainer* child) = 0;
    virtual QPoint getIndexPositionForChild(NodeItemOrderedContainer* child) = 0;

    QPoint getIndexPosition() const;
    void setIndexPosition(QPoint point);
    void setPos(const QPointF &pos);

private:
    NodeItemOrderedContainer* container;
    QPoint indexPosition;
};

#endif // NODEITEMORDEREDCONTAINER_H
