#include "nodeviewitem.h"
#include "edgeviewitem.h"
#include <QDebug>
#include "viewcontroller.h"
#include "../../../modelcontroller/entityfactory.h"


NodeViewItem::NodeViewItem(ViewController *controller, NODE_KIND kind, QString label):ViewItem(controller, GRAPHML_KIND::NODE)
{
    node_kind = kind;
    changeData(KeyName::Kind, EntityFactory::getNodeKindString(kind));
    changeData(KeyName::Label, label);
}

NodeViewItem::NodeViewItem(ViewController *controller, int ID, NODE_KIND kind):ViewItem(controller, ID, GRAPHML_KIND::NODE)
{
    node_kind = kind;
}

NodeViewItem::~NodeViewItem(){
    auto edges_to_delete = edges;
    edges.clear();

    for(auto edge_kind : edges_to_delete.uniqueKeys()){
        for(auto edge : edges_to_delete[edge_kind]){
            edge->disconnectEdge();
        }
    }
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
        nestedVisualEdgeKindsChanged();
    }
}

void NodeViewItem::childRemoved(ViewItem* child){
    if(child && child->isNode()){
        auto node = (NodeViewItem*) child;
        disconnect(node, &NodeViewItem::visualEdgeKindsChanged, this, &NodeViewItem::nestedVisualEdgeKindsChanged);
        nestedVisualEdgeKindsChanged();
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
            

            auto& set = visual_edge_kinds[kind];
            auto& nested_set = nested_edge_kinds[kind];
            auto size = set.size();
            set.insert(direction);
            nested_set.insert(direction);
            
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
            
            if(last_edge && !valid_edge_kinds[kind].contains(direction)){
                //Test the edges map for any more edges in this direction
                auto& set = visual_edge_kinds[kind];
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

bool NodeViewItem::isAncestorOf(NodeViewItem *item)
{
    bool is_ancestor = false;

    if(item){
        is_ancestor = getController()->isNodeAncestor(getID(), item->getID());
    }
    return is_ancestor;
}


/*
QSet<EDGE_KIND> NodeViewItem::getVisualEdgeKinds(){
    auto kinds = visual_edge_kinds.keys() + owned_edge_kinds.keys();
    return kinds.toSet();
}*/

const QSet<EDGE_DIRECTION>& NodeViewItem::getVisualEdgeKindDirections(EDGE_KIND kind){
    return visual_edge_kinds[kind];
}

const QHash<EDGE_KIND, QSet<EDGE_DIRECTION> >& NodeViewItem::getNestedVisualEdges(){
    return nested_edge_kinds;
}

void NodeViewItem::nestedVisualEdgeKindsChanged(){
    QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > new_nested_edge_kinds = visual_edge_kinds;
    
    for(auto child : getDirectChildren()){
        if(child->isNode()){
            auto node = (NodeViewItem*) child;
            const auto& child_map = node->getNestedVisualEdges();
            for(const auto key : child_map.keys()){
                new_nested_edge_kinds[key] += child_map[key];
            }
        }
    }



    //Update our map
    if(nested_edge_kinds != new_nested_edge_kinds){
        nested_edge_kinds = new_nested_edge_kinds;
        emit visualEdgeKindsChanged();
    }
}

void NodeViewItem::setValidEdgeKinds(QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > required_edge_kinds){
    //copy the updated valid edge_kinds
    valid_edge_kinds = required_edge_kinds;

    //Update required_edge_kinds to include our list
    for(const auto& edge_kind : edges.uniqueKeys()){
        for(auto edge : edges.value(edge_kind)){
            auto is_source = edge->getSource() == this;
            auto direction = is_source ? EDGE_DIRECTION::SOURCE : EDGE_DIRECTION::TARGET;
            required_edge_kinds[edge_kind].insert(direction);
        }
    }

    if(required_edge_kinds != visual_edge_kinds){
        visual_edge_kinds = required_edge_kinds;
        nested_edge_kinds.unite(required_edge_kinds);
        //Update
        emit visualEdgeKindsChanged();
    }
}

const QSet<EDGE_DIRECTION>& NodeViewItem::getNestedVisualEdgeKindDirections(EDGE_KIND kind){
    for(const auto key : visual_edge_kinds.keys()){
        nested_edge_kinds[key] += visual_edge_kinds[key];
    }
    return nested_edge_kinds[kind];
}

void NodeViewItem::setNodeTypes(QSet<NODE_TYPE> types){
    node_types = types;
}