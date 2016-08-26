#include "nodeitemcolumnitem.h"
#include <QDebug>
NodeItemColumnItem::NodeItemColumnItem(NodeViewItem *viewItem, NodeItemNew *parentItem):ColumnContainerNodeItem(viewItem, parentItem)
{
    Node::NODE_KIND nodeKind;
    if(viewItem){
        nodeKind = viewItem->getNodeKind();
    }

    QPoint initialPosition;
    switch(nodeKind){
    case Node::NK_OUTEVENTPORT_INSTANCE:
    case Node::NK_OUTEVENTPORT_DELEGATE:
        initialPosition.setX(0);
        break;
    case Node::NK_INEVENTPORT_INSTANCE:
    case Node::NK_INEVENTPORT_DELEGATE:
        initialPosition.setX(2);
        break;
    case Node::NK_ATTRIBUTE_INSTANCE:
        initialPosition.setX(1);
        break;
    default:
        break;
    }

    setIndexPosition(initialPosition);
}
