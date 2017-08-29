#include "nodeviewitem.h"
#include "edgeviewitem.h"
#include <QDebug>
#include "viewcontroller.h"
#include "../../ModelController/entityfactory.h"


NodeViewItem::NodeViewItem(ViewController *controller, NODE_KIND kind, QString label):ViewItem(controller)
{
    nodeKind = kind;
    changeData("kind", EntityFactory::getNodeKindString(kind));
    changeData("label", label);
}

NodeViewItem::NodeViewItem(ViewController *controller, int ID, NODE_KIND kind):ViewItem(controller, ID, GRAPHML_KIND::NODE)
{
    nodeKind = kind;
    changeData("x", 0);
    changeData("y", 0);
    changeData("isExpanded", true);
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

bool NodeViewItem::isInModel(){
    return getViewAspect() != VIEW_ASPECT::WORKERS && getViewAspect() != VIEW_ASPECT::NONE;
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

