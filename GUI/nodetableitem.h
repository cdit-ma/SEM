#ifndef NODETABLEITEM_H
#define NODETABLEITEM_H
#include <QList>
#include <QObject>
class Node;
class NodeTableItem
{
public:
    NodeTableItem(Node* node, NodeTableItem *parent = 0);
    ~NodeTableItem();
    Node* getNode();

    void appendChild(NodeTableItem *child);
    void removeChild(NodeTableItem *child);

    NodeTableItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    NodeTableItem *parent();

private:
    QList<NodeTableItem*> childItems;
    Node* node;
    NodeTableItem *parentItem;
};

#endif // NODETABLEITEM_H
