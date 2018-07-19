#include "nodeviewitem.h"
#include "edgeviewitem.h"
#include <QDebug>
#include "viewcontroller.h"
#include "../../../modelcontroller/entityfactory.h"


NodeViewItem::NodeViewItem(ViewController *controller, NODE_KIND kind, QString label):ViewItem(controller, GRAPHML_KIND::NODE)
{
    node_kind = kind;
    changeData("kind", EntityFactory::getNodeKindString(kind));
    changeData("label", label);
}

NodeViewItem::NodeViewItem(ViewController *controller, int ID, NODE_KIND kind):ViewItem(controller, ID, GRAPHML_KIND::NODE)
{
    node_kind = kind;
    //changeData("isExpanded", true);
}


NODE_KIND NodeViewItem::getNodeKind() const
{
    return node_kind;
}

NodeViewItem *NodeViewItem::getParentNodeViewItem() const
{
    auto parent = getParentItem();
    if(parent && parent->isNode()){
        return (NodeViewItem*) parent;
    }
    return 0;
}

NODE_KIND NodeViewItem::getParentNodeKind() const{
    auto parent_node = getParentNodeViewItem();
    return parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;
}


int NodeViewItem::getParentID()
{
    if(parent_id == -1){
        auto parent = getParentItem();
        if(parent){
            parent_id = parent->getID();
        }
    }
    return parent_id;
}

bool NodeViewItem::isNodeOfType(NODE_TYPE type) const
{
    return node_types.contains(type);
}

void NodeViewItem::childAdded(ViewItem* child){
    if(child && child->isNode()){
        auto node = (NodeViewItem*) child;
        connect(node, &NodeViewItem::visualEdgeKindsChanged, this, &NodeViewItem::nestedVisualEdgeKindsChanged);
        
    }
}

void NodeViewItem::childRemoved(ViewItem* child){
    if(child && child->isNode()){
        auto node = (NodeViewItem*) child;
        disconnect(node, &NodeViewItem::visualEdgeKindsChanged, this, &NodeViewItem::nestedVisualEdgeKindsChanged);
    }
}

void NodeViewItem::addEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        EDGE_KIND kind = edge->getEdgeKind();
        if(!edges.contains(kind, edge)){
            edges.insertMulti(kind, edge);

            //Get the direction
            auto is_source = edge->getSource() == this;
            auto direction = is_source ? EDGE_DIRECTION::SOURCE : EDGE_DIRECTION::TARGET;


            emit edgeAdded(direction, kind, edge->getID());

            if(!owned_edge_kinds.contains(kind, direction)){
                owned_edge_kinds.insert(kind, direction);
                emit visualEdgeKindsChanged();
            }
        }
    }
}

void NodeViewItem::removeEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        EDGE_KIND kind = edge->getEdgeKind();
        if(edges.contains(kind, edge)){
            edges.remove(kind, edge);

            //Get the direction
            auto is_source = edge->getSource() == this;
            auto direction = is_source ? EDGE_DIRECTION::SOURCE : EDGE_DIRECTION::TARGET;

            emit edgeRemoved(direction, kind, edge->getID());

            if(owned_edge_kinds.contains(kind, direction)){
                owned_edge_kinds.remove(kind, direction);
                emit visualEdgeKindsChanged();
            }
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

    if(item){
        is_ancestor = getController()->isNodeAncestor(getID(), item->getID());
    }
    return is_ancestor;
}

void NodeViewItem::addVisualEdgeKind(EDGE_KIND kind, EDGE_DIRECTION direction){
    if(!visual_edge_kinds.contains(kind, direction)){
        visual_edge_kinds.insert(kind, direction);
    }
}

void NodeViewItem::clearVisualEdgeKinds(){
    visual_edge_kinds.clear();
}

QSet<EDGE_KIND> NodeViewItem::getVisualEdgeKinds(){
    auto kinds = visual_edge_kinds.keys().toSet() + owned_edge_kinds.keys().toSet();
    return kinds;
}

QSet<EDGE_DIRECTION> NodeViewItem::getVisualEdgeKindDirections(EDGE_KIND kind){
    auto values = visual_edge_kinds.values(kind).toSet() + owned_edge_kinds.values(kind).toSet();
    return values;
}

QSet<EDGE_KIND> NodeViewItem::getNestedVisualEdgeKinds(){
    auto kinds = getVisualEdgeKinds();

    for(auto child : getDirectChildren()){
        if(child->isNode()){
            auto node = (NodeViewItem*) child;
            kinds += node->getNestedVisualEdgeKinds();
        }
    }
    return kinds;
}

QSet<EDGE_DIRECTION> NodeViewItem::getNestedVisualEdgeKindDirections(EDGE_KIND kind){
    auto max_count = 2;
    auto directions = getVisualEdgeKindDirections(kind);

    for(auto child : getDirectChildren()){
        if(child->isNode()){
            auto node = (NodeViewItem*) child;
            directions += node->getNestedVisualEdgeKindDirections(kind);
            
        }
        if(directions.size() == 2){
            break;
        }
    }

    return directions;
}

void NodeViewItem::setNodeTypes(QSet<NODE_TYPE> types){
    node_types = types;
}