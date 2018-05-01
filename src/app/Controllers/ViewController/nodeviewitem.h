#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "viewitem.h"
#include "../../../modelcontroller/nodekinds.h"
#include "../../../modelcontroller/edgekinds.h"


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

    void clearVisualEdgeKinds();
    void addVisualEdgeKind(EDGE_KIND kind, EDGE_DIRECTION direction);

    QSet<EDGE_KIND> getVisualEdgeKinds();
    QSet<EDGE_DIRECTION> getVisualEdgeKindDirections(EDGE_KIND kind);
    
signals:
    void edgeAdded(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void edgeRemoved(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void visualEdgeKindsChanged();
private:
    QMultiMap<EDGE_KIND, EDGE_DIRECTION> visual_edge_kinds;
    QMultiMap<EDGE_KIND, EDGE_DIRECTION> owned_edge_kinds;

    QMultiMap<EDGE_KIND, EdgeViewItem*> edges;
    NODE_KIND nodeKind;
    int parent_id = -1;
};
#endif // VIEWITEM_H
