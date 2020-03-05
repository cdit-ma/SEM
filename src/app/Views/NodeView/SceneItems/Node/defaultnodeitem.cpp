#include "defaultnodeitem.h"

DefaultNodeItem::DefaultNodeItem(NodeViewItem *viewItem, NodeItem *parentItem)
        : BasicNodeItem(viewItem, parentItem)
{
    setExpandEnabled(true);
    setSortOrdered(false);
    reloadRequiredData();
}
