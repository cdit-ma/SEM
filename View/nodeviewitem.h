#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "../Model/node.h"
#include "viewitem.h"
class EdgeViewItem;

class NodeViewItem: public ViewItem
{
    Q_OBJECT
public:
    NodeViewItem(ViewController* controller, int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> _properties);


    bool isNodeOfType(Node::NODE_TYPE type) const;
    Node::NODE_KIND getNodeKind() const;
    NodeViewItem* getParentNodeViewItem();
    VIEW_ASPECT getViewAspect();
    int getParentID();

    void addEdgeItem(EdgeViewItem* edge);
    void removeEdgeItem(EdgeViewItem* edge);
    QList<EdgeViewItem*> getEdges(Edge::EDGE_KIND edgeKind = Edge::EC_NONE) const;
    bool gotEdge(Edge::EDGE_KIND edgeKind = Edge::EC_NONE) const;

    bool isInModel();
    QString getTreeIndex();


    bool isAncestorOf(NodeViewItem* item);
signals:
    void edgeAdded(Edge::EDGE_KIND edgeKind);
    void edgeRemoved(Edge::EDGE_KIND edgeKind);

private:
    QMultiMap<Edge::EDGE_KIND, EdgeViewItem*> edges;
    Node::NODE_KIND nodeKind;
};
#endif // VIEWITEM_H
