#ifndef NODEITEMTREEITEM_H
#define NODEITEMTREEITEM_H

#include <QList>
#include <QVariant>
#include "nodeitem.h"


class NodeItemTreeItem
{
public:
    NodeItemTreeItem();
    NodeItemTreeItem(NodeItem* nodeItem, NodeItemTreeItem* parent);
    ~NodeItemTreeItem();

    void appendChild(NodeItemTreeItem* child);
    void setParent(NodeItemTreeItem* parent);

    NodeItemTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    NodeItemTreeItem* parent();

private:
    QList<NodeItemTreeItem*> childItems;
    NodeItemTreeItem* parentItem;
    NodeItem* nodeItem;
};


#endif // NODEITEMTREEITEM_H
