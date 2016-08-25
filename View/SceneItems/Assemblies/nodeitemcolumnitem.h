#ifndef NODEITEMCOLUMNITEM_H
#define NODEITEMCOLUMNITEM_H

#include "nodeitemcolumncontainer.h"
class NodeItemColumnItem: public NodeItemColumnContainer
{
    Q_OBJECT
public:
    NodeItemColumnItem(NodeViewItem *viewItem, NodeItemNew *parentItem);
};

#endif // NODEITEMCOLUMNITEM_H
