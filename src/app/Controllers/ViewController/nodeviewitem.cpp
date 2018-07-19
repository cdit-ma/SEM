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
        const auto& kind = edge->getEdgeKind();

        if(!edges[kind].contains(edge)){
            edges[kind].insert(edge);

            //Get the direction
            auto is_source = edge->getSource() == this;
            auto direction = is_source ? EDGE_DIRECTION::SOURCE : EDGE_DIRECTION::TARGET;


            emit edgeAdded(direction, kind, edge->getID());
            

            auto& set = owned_edge_kinds[kind];
            auto size = set.size();
            set.insert(direction);
            
            if(set.size() > size){
                emit visualEdgeKindsChanged();
            }
        }
    }
}

void NodeViewItem::removeEdgeItem(EdgeViewItem *edge)
{
    if(edge){
        const auto& kind = edge->getEdgeKind();
        
        if(edges[kind].contains(edge)){
            edges[kind].remove(edge);

            //Get the direction
            auto is_source = edge->getSource() == this;
            auto direction = is_source ? EDGE_DIRECTION::SOURCE : EDGE_DIRECTION::TARGET;

            emit edgeRemoved(direction, kind, edge->getID());

            bool last_edge = true;
            for(auto e : edges[kind]){
                auto e_source = e->getSource() == this;
                auto e_dir = e_source ? EDGE_DIRECTION::SOURCE : EDGE_DIRECTION::TARGET;
                if(e_dir == direction){
                    last_edge = false;
                    break;
                }
            }
            
            if(last_edge){
                //Test the edges map for any more edges in this directio
                auto& set = owned_edge_kinds[kind];
                set.remove(direction);
                emit visualEdgeKindsChanged();
            }
        }
    }
}

QSet<EdgeViewItem *> NodeViewItem::getEdges() const
{
    QSet<EdgeViewItem *> e;
    for(const auto& edge_kind : edges.keys()){
        e += edges[edge_kind];
    }
    return e;
}

QSet<EdgeViewItem *> NodeViewItem::getEdges(EDGE_KIND edge_kind) const
{
    if(edge_kind == EDGE_KIND::NONE){
        return getEdges();
    }else{
        return edges[edge_kind];
    }
}

bool NodeViewItem::gotEdge(EDGE_KIND edge_kind) const
{
    if(edge_kind == EDGE_KIND::NONE){
        for(const auto& edge_kind : edges.keys()){
            if(edges[edge_kind].size()){
                return true;
            }
        }
        return false;
    }else{
        return edges[edge_kind].size();
    }
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
    visual_edge_kinds[kind].insert(direction);
}

void NodeViewItem::clearVisualEdgeKinds(){
    visual_edge_kinds.clear();
}

QSet<EDGE_KIND> NodeViewItem::getVisualEdgeKinds(){
    auto kinds = visual_edge_kinds.keys() + owned_edge_kinds.keys();
    return kinds.toSet();
}

QSet<EDGE_DIRECTION> NodeViewItem::getVisualEdgeKindDirections(EDGE_KIND kind){
    return visual_edge_kinds.value(kind) + owned_edge_kinds.value(kind);
}

QSet<EDGE_KIND> NodeViewItem::getNestedVisualEdgeKinds(){
    QSet<EDGE_KIND> kinds= getVisualEdgeKinds();

    for(auto child : getDirectChildren()){
        if(child->isNode()){
            auto node = (NodeViewItem*) child;
            kinds += node->getNestedVisualEdgeKinds();
        }
    }
    return kinds;
}

QSet<EDGE_DIRECTION> NodeViewItem::getNestedVisualEdgeKindDirections(EDGE_KIND kind){
    QSet<EDGE_DIRECTION> set = getVisualEdgeKindDirections(kind);
    for(auto child : getDirectChildren()){
        if(set.size() == 2){
            break;
        }else if(child->isNode()){
            auto node = (NodeViewItem*) child;
            set += node->getNestedVisualEdgeKindDirections(kind);
        }
    }
    return set;
}

void NodeViewItem::setNodeTypes(QSet<NODE_TYPE> types){
    node_types = types;
}