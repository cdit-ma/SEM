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

    friend class ViewController;
public:
    NodeViewItem(ViewController* controller, NODE_KIND kind, QString label);
    NodeViewItem(ViewController* controller, int ID, NODE_KIND kind);


    NODE_KIND getNodeKind() const;
    NodeViewItem* getParentNodeViewItem() const;
    NODE_KIND getParentNodeKind() const;
    int getParentID();
    bool isNodeOfType(NODE_TYPE type) const;
    void addEdgeItem(EdgeViewItem* edge);
    void removeEdgeItem(EdgeViewItem* edge);

    QSet<EdgeViewItem*> getEdges() const;
    QSet<EdgeViewItem*> getEdges(EDGE_KIND edgeKind) const;
    bool gotEdge(EDGE_KIND edgeKind) const;
    
    bool isAncestorOf(NodeViewItem* item);

    void clearVisualEdgeKinds();
    void addVisualEdgeKind(EDGE_KIND kind, EDGE_DIRECTION direction);

    QSet<EDGE_KIND> getVisualEdgeKinds();
    QSet<EDGE_DIRECTION> getVisualEdgeKindDirections(EDGE_KIND kind);

    QSet<EDGE_KIND> getNestedVisualEdgeKinds();
    QSet<EDGE_DIRECTION> getNestedVisualEdgeKindDirections(EDGE_KIND kind);

    

protected:
    void setNodeTypes(QSet<NODE_TYPE> types);
    void childAdded(ViewItem* child);
    void childRemoved(ViewItem* child);
    
signals:
    void edgeAdded(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void edgeRemoved(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void visualEdgeKindsChanged();
    void nestedVisualEdgeKindsChanged();
private:
    void childVisualEdgeKindsChanged();
    
    QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > visual_edge_kinds;
    QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > owned_edge_kinds;
    QHash<EDGE_KIND, QSet<EdgeViewItem*> > edges;

    QSet<NODE_TYPE> node_types;

    //QMultiMap<EDGE_KIND, EdgeViewItem*> edges;
    NODE_KIND node_kind;
    int parent_id = -1;
};
#endif // VIEWITEM_H
