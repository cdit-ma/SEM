#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "viewitem.h"
#include "../../Model/nodekinds.h"

class EdgeViewItem;

class NodeViewItem: public ViewItem
{
    Q_OBJECT
public:
    NodeViewItem(ViewController* controller, int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> _properties);
    NodeViewItem(ViewController* controller, NODE_KIND kind, QString label);
    
    NodeViewItem(ViewController* controller, int ID, NODE_KIND kind);


    NODE_KIND getNodeKind() const;
    NodeViewItem* getParentNodeViewItem();
    VIEW_ASPECT getViewAspect();
    int getParentID();
    bool isNodeOfType(NODE_TYPE type);

    void addEdgeItem(EdgeViewItem* edge);
    void removeEdgeItem(EdgeViewItem* edge);
    QList<EdgeViewItem*> getEdges(Edge::EDGE_KIND edgeKind = Edge::EC_NONE) const;
    bool gotEdge(Edge::EDGE_KIND edgeKind = Edge::EC_NONE) const;
    
    bool isAncestorOf(NodeViewItem* item);
signals:
    void edgeAdded(Edge::EDGE_KIND edgeKind);
    void edgeRemoved(Edge::EDGE_KIND edgeKind);

private:
    QMultiMap<Edge::EDGE_KIND, EdgeViewItem*> edges;
    NODE_KIND nodeKind;
    VIEW_ASPECT aspect = VA_NONE;
    int parent_id = -1;
};
#endif // VIEWITEM_H
