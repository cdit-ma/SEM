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

bool NodeViewItem::isNodeOfType(Node::NODE_TYPE type) const
{
    if(hasProperty("nodeTypes")){
        int types = getProperty("nodeTypes").toInt();
        return types & type;
    }
    return false;
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

void NodeViewItem::addEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        Edge::EDGE_KIND kind = edge->getEdgeKind();
        if(!edges.contains(kind, edge)){
            edges.insertMulti(kind, edge);
        }
    }
}

void NodeViewItem::removeEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        Edge::EDGE_KIND kind = edge->getEdgeKind();
        if(edges.contains(kind, edge)){
            edges.remove(kind, edge);
        }
    }
}

QList<EdgeViewItem *> NodeViewItem::getEdges(Edge::EDGE_KIND edgeKind) const
{
    return edgeKind == Edge::EC_NONE ? edges.values() : edges.values(edgeKind);
}

bool NodeViewItem::gotEdge(Edge::EDGE_KIND edgeKind) const
{
    return edgeKind == Edge::EC_NONE ? !edges.values().isEmpty() : edges.values(edgeKind).isEmpty();
}

bool NodeViewItem::isInModel()
{
    bool inModel = false;
    if(hasProperty("inModel")){
        inModel = getProperty("inModel").toBool();
    }
    return inModel;
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

