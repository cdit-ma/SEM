#include "nodeviewitem.h"
#include "edgeviewitem.h"
#include <QDebug>
#include "viewcontroller.h"
#include "../../Model/nodekinds.h"
#include "../../Model/edgekinds.h"

NodeViewItem::NodeViewItem(ViewController* controller, int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties):ViewItem(controller, ID, entityKind, kind, data, properties)
{
    nodeKind = NODE_KIND::NONE;

    if(properties.contains("kind")){
        nodeKind = (NODE_KIND) properties["kind"].toInt();
    }
}

NodeViewItem::NodeViewItem(ViewController *controller, NODE_KIND kind, QString label):ViewItem(controller)
{
    nodeKind = kind;
    changeData("kind", EntityFactory::getNodeKindString(kind));
    changeData("label", label);
}

NodeViewItem::NodeViewItem(ViewController *controller, int ID, NODE_KIND kind):ViewItem(controller, ID, ENTITY_KIND::EK_NODE)
{
    nodeKind = kind;
}


NODE_KIND NodeViewItem::getNodeKind() const
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
    //Get Once
    if(aspect == VA_NONE){
        aspect = getController()->getNodeViewAspect(getID());
    }
    return aspect;
}

int NodeViewItem::getParentID()
{
    if(parent_id == -1){
        parent_id = getController()->getNodeParentID(getID());
    }
    return parent_id;
}

bool NodeViewItem::isNodeOfType(NODE_TYPE type)
{
    bool is_type = false;
    if(getController()){
        is_type = getController()->isNodeOfType(getID(), type);
    }
    return is_type;
}

void NodeViewItem::addEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        EDGE_KIND kind = edge->getEdgeKind();
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
        EDGE_KIND kind = edge->getEdgeKind();
        if(edges.contains(kind, edge)){
            edges.remove(kind, edge);
            //this->unregisterObject(edge);
            emit edgeRemoved(kind);
            //edge-edge->registerObject(this);
        }
    }
}

QList<EdgeViewItem *> NodeViewItem::getEdges() const
{
    return edges.values();
}

QList<EdgeViewItem *> NodeViewItem::getEdges(EDGE_KIND edgeKind) const
{
    return edgeKind == EDGE_KIND::NONE ? edges.values() : edges.values(edgeKind);
}

bool NodeViewItem::gotEdge(EDGE_KIND edgeKind) const
{
    return edgeKind == EDGE_KIND::NONE ? !edges.values().isEmpty() : !edges.values(edgeKind).isEmpty();
}


bool NodeViewItem::isAncestorOf(NodeViewItem *item)
{
    bool is_ancestor = false;

    if(getController() && item){
        is_ancestor = getController()->isNodeAncestor(getID(), item->getID());
    }
    return is_ancestor;
}

