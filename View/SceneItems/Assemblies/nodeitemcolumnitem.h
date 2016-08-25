#ifndef NODEITEMCOLUMNITEM_H
#define NODEITEMCOLUMNITEM_H

#include "../nodeitemcontainer.h"
#include "nodeitemcolumncontainer.h"

class NodeItemColumnItem: public NodeItemContainer
{
    Q_OBJECT
public:
    NodeItemColumnItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    NodeItemColumnContainer* getColumnParent();
    int getColumn();
    int getRow();


    // EntityItemNew interface
public:
    void setPos(const QPointF &pos);
private:
    NodeItemColumnContainer* container;
    int column;
    int row;
};

#endif // NODEITEMCOLUMNITEM_H
