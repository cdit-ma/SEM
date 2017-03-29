#include "nodeviewitem.h"
#include "edgeviewitem.h"
#include <QDebug>


NodeViewItem::NodeViewItem(ViewController* controller, int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties):ViewItem(controller, ID, entityKind, kind, data, properties)
{
    nodeKind = Node::NK_NONE;

    if(properties.contains("kind")){
        nodeKind = (Node::NODE_KIND) properties["kind"].toInt();
    }
}

NodeViewItem::NodeViewItem(ViewController *controller, Node::NODE_KIND kind, QString label):ViewItem(controller)
{
    nodeKind = kind;
    changeData("kind", NodeFactory::getNodeKindString(kind));
    changeData("label", label);

}


Node::NODE_KIND NodeViewItem::getNodeKind() const
{
    return nodeKind;
}

NodeViewItem *NodeViewItem::getParentNodeViewItem()
{
    ViewItem* parent = getParentItem();
    if(parent && parent->isNode()){
        return (NodeViewItem*) parent;
    }
    return 0;
}

VIEW_ASPECT NodeViewItem::getViewAspect()
{
    VIEW_ASPECT aspect = VA_NONE;
    if(hasProperty("viewAspect")){
        int val = getProperty("viewAspect").toInt();
        aspect = (VIEW_ASPECT) val;
    }
    return aspect;
}

int NodeViewItem::getParentID()
{
    int ID = -1;
    if(hasProperty("parentID")){
        ID = getProperty("parentID").toInt();
    }
    return ID;
}

bool NodeViewItem::isNodeOfType(Node::NODE_TYPE type) const
{
    if(hasProperty("nodeTypes")){
        int types = getProperty("nodeTypes").toInt();
        return types & type;
    }
    return false;
}

void NodeViewItem::addEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        Edge::EDGE_KIND kind = edge->getEdgeKind();
        if(!edges.contains(kind, edge)){
            //this->registerObject(edge);
            edges.insertMulti(kind, edge);
            emit edgeAdded(kind);
        }
    }
}

void NodeViewItem::removeEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        Edge::EDGE_KIND kind = edge->getEdgeKind();
        if(edges.contains(kind, edge)){
            edges.remove(kind, edge);
            //this->unregisterObject(edge);
            emit edgeRemoved(kind);
            //edge-edge->registerObject(this);
        }
    }
}

QList<EdgeViewItem *> NodeViewItem::getEdges(Edge::EDGE_KIND edgeKind) const
{
    return edgeKind == Edge::EC_NONE ? edges.values() : edges.values(edgeKind);
}

bool NodeViewItem::gotEdge(Edge::EDGE_KIND edgeKind) const
{
    return edgeKind == Edge::EC_NONE ? !edges.values().isEmpty() : !edges.values(edgeKind).isEmpty();
}

QString NodeViewItem::getTreeIndex()
{
    QString index;
    if(hasProperty("treeIndex")){
        index = getProperty("treeIndex").toString();
    }
    return index;
}



bool NodeViewItem::isAncestorOf(NodeViewItem *item)
{
	if(!item){
		return false;
	}
    QString thisTree = getTreeIndex();
    QString thatTree = item->getTreeIndex();

    if(this == item){
        return true;
    }

    if(thisTree.size() >= thatTree.size()){
        return false;
    }

    return thatTree.startsWith(thisTree);
}

