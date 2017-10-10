#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "viewitem.h"
#include "../../ModelController/nodekinds.h"
#include "../../ModelController/edgekinds.h"


class EdgeViewItem;

class NodeViewItem: public ViewItem
{
    Q_OBJECT
public:
    NodeViewItem(ViewController* controller, NODE_KIND kind, QString label);
    NodeViewItem(ViewController* controller, int ID, NODE_KIND kind);


    NODE_KIND getNodeKind() const;
    NodeViewItem* getParentNodeViewItem();
    int getParentID();
    bool isNodeOfType(NODE_TYPE type);
    bool isInModel();
    void addEdgeItem(EdgeViewItem* edge);
    void removeEdgeItem(EdgeViewItem* edge);

    QList<EdgeViewItem*> getEdges() const;
    QList<EdgeViewItem*> getEdges(EDGE_KIND edgeKind) const;
    bool gotEdge(EDGE_KIND edgeKind) const;
    
    bool isAncestorOf(NodeViewItem* item);
signals:
    void edgeAdded(EDGE_KIND edgeKind);
    void edgeRemoved(EDGE_KIND edgeKind);

private:
    QMultiMap<EDGE_DIRECTION, EDGE_KIND> required_edge_kinds;

    QMultiMap<EDGE_KIND, EdgeViewItem*> edges;
    NODE_KIND nodeKind;
    int parent_id = -1;
};
#endif // VIEWITEM_H
