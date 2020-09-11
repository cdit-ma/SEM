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
    NodeViewItem(ViewController* controller, NODE_KIND kind, const QString& label);
    NodeViewItem(ViewController* controller, int ID, NODE_KIND kind);
    ~NodeViewItem() final;

    NODE_KIND getNodeKind() const;
    NodeViewItem* getParentNodeViewItem() const;
    NODE_KIND getParentNodeKind() const;
    
    int getParentID();
    bool isNodeOfType(NODE_TYPE type) const;
    void addEdgeItem(EdgeViewItem* edge);
    void removeEdgeItem(EdgeViewItem* edge);

    bool isInstance() const;

    QSet<EdgeViewItem*> getEdges() const;
    QSet<EdgeViewItem*> getEdges(EDGE_KIND edgeKind) const;

    QSet<NodeViewItem*> getChildrenNodeViewItems() const;
    
    bool isAncestorOf(NodeViewItem* item);

    const QSet<EDGE_DIRECTION>& getVisualEdgeKindDirections(EDGE_KIND kind);
    const QSet<EDGE_DIRECTION>& getNestedVisualEdgeKindDirections(EDGE_KIND kind);

    void setValidEdgeKinds(QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > required_edge_kinds);

protected:
    void nestedVisualEdgeKindsChanged();
    void setNodeTypes(const QSet<NODE_TYPE>& types);
    void childAdded(ViewItem* child) override;
    void childRemoved(ViewItem* child) override;

    const QHash<EDGE_KIND, QSet<EDGE_DIRECTION>>& getNestedVisualEdges();
    
signals:
    void edgeAdded(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void edgeRemoved(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void visualEdgeKindsChanged();
    void nestedVisualEdgeKindsChanged2();
    
private:
    QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > nested_edge_kinds;
    QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > valid_edge_kinds;
    QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > visual_edge_kinds;
    QHash<EDGE_KIND, QSet<EdgeViewItem*> > edges;

    QSet<NODE_TYPE> node_types;

    NODE_KIND node_kind;
    int parent_id = -1;
};

#endif // NODEVIEWITEM_H
