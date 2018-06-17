#include "node.h"
#include "data.h"
#include "edge.h"
#include <algorithm>
#include <QDebug>
#include <QStringBuilder>
#include <QByteArray>
#include <QStack>
#include "../entityfactorybroker.h"
#include "../entityfactoryregistrybroker.h"
#include "../entityfactoryregistrybroker.h"

#include "InterfaceDefinitions/datanode.h"

#include "../nodekinds.h"
#include "../edgekinds.h"
#include "Keys/indexkey.h"
#include "Keys/typekey.h"

Node::Node(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp_node) : Entity(broker, GRAPHML_KIND::NODE){
    //Setup State
    node_kind_ = node_kind;

    if(is_temp_node){
        return;
    }

    //Attach default data
    broker.AttachData(this, "kind", QVariant::String, ProtectedState::PROTECTED, broker.GetNodeKindString(node_kind));
    broker.AttachData(this, "label", QVariant::String, ProtectedState::UNPROTECTED, broker.GetNodeKindString(node_kind));
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED, -1);

    connect(this, &Node::acceptedEdgeKindsChanged, [=](Node* node){
        getFactoryBroker().AcceptedEdgeKindsChanged(node);
    });
}

Node::~Node()
{
    //Deregister first 
    getFactoryBroker().DeregisterNode(this);

    if(parent_node_){
        parent_node_->removeChild(this);
    }
}


int Node::getDepth() const{
    return tree_index_.size();
}

QList<int> Node::getTreeIndex()
{
    return tree_index_;
}

NODE_KIND Node::getNodeKind() const
{
    return node_kind_;
}





void Node::SetEdgeRuleActive(EdgeRule rule, bool active){
    if(active){
        active_edge_rules_.insert(rule);
    }else{
        active_edge_rules_.remove(rule);
    }
}


bool Node::IsEdgeRuleActive(EdgeRule rule) const{
    return active_edge_rules_.contains(rule);
}

bool Node::canCurrentlyAcceptEdgeKind(EDGE_KIND edge_kind, Node* dst) const{
    if(canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::SOURCE) == false){
        return false;
    }

    if(dst->canCurrentlyAcceptEdgeKind(edge_kind, EDGE_DIRECTION::TARGET) == false){
        return false;
    }

    return true;
}

/**
 * @brief Node::addValidEdgeType Add's an Edge class as a type of edge this node should check in canConnect()
 * @param validEdge
 */
bool Node::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    
    switch(edge_kind){
        case EDGE_KIND::DEFINITION:{
            auto parent_node = getParentNode();

            auto node_kind = getNodeKind();
            auto dst_node_kind = dst->getNodeKind();
            
            if(isValidDefinitionKind(dst_node_kind) == false){
                return false;
            }

            if(isInstance()){
                if(dst->isValidInstanceKind(node_kind) == false){
                    return false;
                }
            }else if(isImpl()){
                if(dst->isValidImplKind(node_kind) == false){
                    return false;
                }

                auto impl_count = dst->getImplementations().count();

                //Only allow one Impl
                if(impl_count > 0 && !IsEdgeRuleActive(EdgeRule::ALLOW_MULTIPLE_IMPLEMENTATIONS)){
                    return false;
                }
            }

            auto check_valid_definitions = parent_node_ && parent_node_->getDefinition();

            if(check_valid_definitions || IsEdgeRuleActive(EdgeRule::ALWAYS_CHECK_VALID_DEFINITIONS)){
                auto allow_external  = IsEdgeRuleActive(EdgeRule::ALLOW_EXTERNAL_DEFINITIONS);

                //Ignore running these checks if we are allowing external definitions
                if(!allow_external){
                    bool is_child = false;
                    for(auto parent : getParentNodesForValidDefinition()){
                        if(dst->getParentNode() == parent){
                            is_child = true;
                            break;
                        }
                    }
                    
                    if(!is_child){
                        return false;
                    }
                }
            }


            //Allow Cyclic Links if we are not a Definition
            if(isDefinition() && indirectlyConnectedTo(dst)){
                return false;
            }

        break;
    }
    case EDGE_KIND::DATA:{
        break;
    }
    case EDGE_KIND::QOS:{
        if(dst->getNodeKind() != NODE_KIND::QOS_DDS_PROFILE){
            return false;
        }
        break;
    }
    case EDGE_KIND::DEPLOYMENT:{
        if(!dst->isNodeOfType(NODE_TYPE::HARDWARE)){
            return false;
        }
        break;
    }
    default:
        break;
    }

    if(dst == this){
        //Don't allow edges to self.
        return false;
    }

    if(gotEdgeTo(dst, edge_kind)){
        //Don't allow multi edges of the same kind.
        return false;
    }
    return true;
}

bool Node::isNodeOfType(NODE_TYPE type) const
{
    return node_types_.contains(type);
}


void Node::setNodeType(NODE_TYPE type)
{
    if(!node_types_.contains(type)){
        node_types_.insert(type);
        emit typesChanged();
    }
}

void Node::removeNodeType(NODE_TYPE type)
{
    if(node_types_.contains(type)){
        node_types_.remove(type);
        emit typesChanged();
    }
}



Node *Node::getCommonAncestor(Node *dst)
{
    int height = getDepthFromCommonAncestor(dst);

    if(height != -1){
        return getParentNode(height);
    }
    return 0;
}

int Node::getDepthFromCommonAncestor(Node *dst)
{
    QList<int> tree1 = getTreeIndex();
    QList<int> tree2;

    if(dst){
        tree2 = dst->getTreeIndex();
    }else{
        return -1;
    }

    int i = 0;
    for(i = 0; i < tree1.size() && i <tree2.size(); i++){
        if(tree1.at(i) != tree2.at(i)){
            break;
        }
    }
    //Get invert of the size
    i = tree1.size() - i;

    return i;
}


void Node::setTop(int index)
{
    branch_ = index;
    updateTreeIndex({});
}
int Node::getBranch(){
    return branch_;
}

void Node::updateTreeIndex(QList<int> parent_tree_index){
    tree_index_ = parent_tree_index;
    tree_index_ << branch_;
    
    for(auto child : getChildren(0)){
        child->updateTreeIndex(getTreeIndex());
    }
}

void Node::updateViewAspect(VIEW_ASPECT parent_view_aspect){
    if(view_aspect_ == VIEW_ASPECT::NONE){
        view_aspect_ = parent_view_aspect;
        //Update our child
        for(auto child : getChildren(0)){
            child->updateViewAspect(getViewAspect());
        }
    }
}

VIEW_ASPECT Node::getViewAspect() const
{
    return view_aspect_;
}

QList<Node*> Node::getParentNodes(int depth){
    QList<Node*> parents;
    Node* node = this;

    while(depth == -1 || depth-- > 0){
        node = node->getParentNode();
        if(node){
            parents.push_back(node);
        }else{
            break;
        }
    }
    return parents;
}

Node* Node::getParentNode(int depth) const{
    if(depth <= 0){
        return 0;
    }
    auto parent_node = getParentNode();
    
    while(--depth > 0){
        if(parent_node){
            parent_node = parent_node->getParentNode();
        }else{
            break;
        }
    }
    return parent_node;

}
Node* Node::getParentNode() const{
    return parent_node_;
}

int Node::getParentNodeID()
{
    if(getParentNode()){
        return getParentNode()->getID();
    }
    return -1;
}

bool Node::canAdoptChild(Node *node)
{
    if(!node){
        return false;
    }

    if(!canAcceptNodeKind(node)){
        return false;
    }

    //Check for Cyclic contains.
    if(node == this){
#ifdef DEBUG_MODE
        qWarning() << "Cannot Adopt self.";
#endif
        return false;
    }

    if(containsChild(node) || isAncestorOf(node) || isDescendantOf(node)){
#ifdef DEBUG_MODE
        qWarning() << "Cannot Adopt Ancestor/Descendant";
#endif
        return false;
    }

    if(node->getDefinition()){
        return false;
    }

    return true;
}



bool Node::addChild(Node *child)
{
    if(child && !containsChild(child) && canAdoptChild(child)){
        if(child->getParentNode()){
            qCritical() << toString() << "ADDING A CHILD: " << child->toString();
        }
        auto node_kind = child->getNodeKind();
        children_.insert(node_kind, child);

        child->setParentNode(this, branch_count_++);

        if(gotData("uuid")){
            auto data = getData("uuid");
            auto key = data->getKey();
            if(!child->gotData("uuid")){
                //Construct a new piece of data
                getFactoryBroker().AttachData(child, key, ProtectedState::PROTECTED);
            }
        }
        childAdded(child);
        return true;
    }
    return false;
}

bool Node::setAsRoot(int root_index)
{
    if(!parent_node_ && tree_index_.length() == 0){
        setTop(root_index);
        return true;
    }
    return false;
}


bool Node::indirectlyConnectedTo(Node *node)
{
    auto parent = getParentNode();
    auto parent_definition = parent ? parent->getDefinition(true) : 0;

    QStack<Node*> items;
    items += node;

    while(!items.isEmpty()){
        auto item = items.takeFirst();

        if(item){
            if(item == parent){
                return true;
            }

            auto item_definition = item->getDefinition(true);
            if(item_definition){
                if(item_definition == parent){
                    return true;
                }

                if(item_definition == parent_definition){
                    return true;
                }
            }

            for(auto node : item->getChildren(0)){
                items += node;
            }
        }
    }

    return false;
}

bool Node::containsChild(Node *child)
{
    auto node_kind = child ? child->getNodeKind() : NODE_KIND::NONE;
    return children_.contains(node_kind, child);
}

QList<Node *> Node::getChildren(int depth)
{
    auto child_list = getOrderedChildNodes();

    //While we still have Children, Recurse
    if(depth != 0){
        for(auto child : getChildren(0)){
            child_list += child->getChildren(depth - 1);
        }
    }
    return child_list;
}

QList<Edge*> Node::getEdges(int depth){
    return getEdgesOfKind(EDGE_KIND::NONE, depth);
}

int Node::getEdgeOfKindCount(EDGE_KIND edge_kind){
    return edges_.count(edge_kind);
}

int Node::getEdgeCount(){
    return edges_.count();
}

QList<Edge *> Node::getEdgesOfKind(EDGE_KIND edge_kind, int depth)
{
    QSet<Edge*> edge_set;

    if(edge_kind == EDGE_KIND::NONE){
        edge_set += edges_.values().toSet();
    }else{
        edge_set += edges_.values(edge_kind).toSet();
    }

    //While we still have Children, Recurse
    if(depth != 0){
        for(auto child : getChildren(0)){
            edge_set += child->getEdgesOfKind(edge_kind, depth - 1).toSet();
        }
    }

    auto edge_list = edge_set.toList();
    std::sort(edge_list.begin(), edge_list.end(), Edge::SortByKind);
    return edge_list;
}



QList<Node *> Node::getSiblings()
{   
    QList<Node*> siblings;
    if(getParentNode()){
        siblings = getParentNode()->getOrderedChildNodes();
        siblings.removeAll(this);
    }
    return siblings;
}


QList<Node *> Node::getChildrenOfKinds(QSet<NODE_KIND> kinds, int depth){
    QList<Node*> returnableList;

    for(auto child : getChildren(depth)){
        if(kinds.contains(child->getNodeKind())){
            returnableList.append(child);
        }
    }
    return returnableList;
}

QList<Node *> Node::getChildrenOfKind(NODE_KIND kind, int depth)
{
    return getChildrenOfKinds({kind}, depth);
}

int Node::getChildrenCount()
{
    return children_.size();
}

int Node::getChildrenOfKindCount(NODE_KIND kind){
    return children_.count(kind);
}

void Node::childRemoved(Node* child){
    IndexKey::RevalidateChildrenIndex(this);
}


bool Node::removeChild(Node *child)
{
    int removeCount = 0;

    if(child){
        auto node_kind = child->getNodeKind();
        removeCount = children_.remove(node_kind, child);
    }

    if(removeCount > 0){
        childRemoved(child);
        return true;
    }
    return false;
}


bool Node::ancestorOf(Node *node)
{
    const auto& a_tree_index = getTreeIndex();
    const auto& b_tree_index = node->getTreeIndex();

    //Not an ancestor of ourselves
    if(this == node){
        return true;
    }

    //If either of tree's aren't setup, we can't be an ancestor
    if(a_tree_index.size() == 0 || b_tree_index.size() == 0){
        return false;
    }

    //If this node is shallower than the comparison, has to be false
    if(a_tree_index.size() > b_tree_index.size()){
        return false;
    }

    for(int i = 0; i < a_tree_index.size(); i++){
        if(a_tree_index.at(i) != b_tree_index.at(i)){
            return false;
        }
    }
    return true;
}

bool Node::ancestorOf(Edge *edge)
{
    auto src = edge->getSource();
    if(ancestorOf(src)){
        return true;
    }
    auto dst = edge->getDestination();
    if(ancestorOf(dst)){
        return true;
    }
    return false;

    return getEdges(-1).contains(edge);
}

bool Node::isAncestorOf(GraphML *item)
{
    switch(item->getGraphMLKind()){
    case GRAPHML_KIND::NODE:
        return ancestorOf((Node*)item);
    case GRAPHML_KIND::EDGE:
        return ancestorOf((Edge*)item);
    default:
        return false;
    }
}


bool Node::isDescendantOf(Node *b)
{   
    auto b_tree_index = b->getTreeIndex();

    //Not an Ancestor of ourselves
    if(this == b){
        return false;
    }

    //If either of tree's aren't setup, we can't be an ancestor
    if(tree_index_.size() == 0 || b_tree_index.size() == 0){
        return false;
    }

    if(this->tree_index_.size() < b_tree_index.size()){
        return false;
    }

    for(int i = 0; i < b_tree_index.size(); i++){
        if(tree_index_.at(i) != b_tree_index.at(i)){
            return false;
        }
    }
    return true;
}



Edge* Node::getEdgeTo(Node *node, EDGE_KIND edge_kind)
{
    for(auto edge : getEdgesOfKind(edge_kind, 0)){
        if(edge->isConnected(node)){
            return edge;
        }
    }
    return 0;
}


bool Node::gotEdgeTo(Node *node, EDGE_KIND edge_kind)
{
    return getEdgeTo(node, edge_kind);
}



bool Node::containsEdge(Edge *edge)
{
    if(edge){
        return edges_.contains(edge->getEdgeKind(), edge);
    }
    return false;
}



QList<Key *> Node::getKeys(int depth)
{
    
    QSet<Data*> datas;

    datas += getData().toSet();

    for(auto node : getChildren(depth)){
        datas += node->getData().toSet();
    }

    for(auto edge : getEdgesOfKind(EDGE_KIND::NONE, depth)){
        datas += edge->getData().toSet();
    }

    QSet<Key*> keys;
    for(auto data : datas){
        keys +=  data->getKey();
    }
    return keys.toList();
}


bool Node::isDefinition() const
{
    return isNodeOfType(NODE_TYPE::DEFINITION);
}

bool Node::isInstance() const
{
    return isNodeOfType(NODE_TYPE::INSTANCE);
}

bool Node::isInstanceImpl() const
{
    return isInstance() || isImpl();
}

bool Node::isAspect() const
{
    return isNodeOfType(NODE_TYPE::ASPECT);
}

bool Node::isImpl() const
{
    return isNodeOfType(NODE_TYPE::IMPLEMENTATION);
}

QSet<NODE_TYPE> Node::getTypes() const
{
    return node_types_;
}

void Node::setDefinition(Node *def)
{
    if(isImpl() || isInstance()){
        definition_ = def;
        BindDefinitionToInstance(definition_, this, true);

        //If have a definition, 
        bool can_have_definition_edge = false;

        if(definition_){
            if(isInstance()){
                can_have_definition_edge = true;
            }
        }else{
            can_have_definition_edge = true;
        }

        definitionSet(definition_);
    }
}

Node *Node::getDefinition(bool recurse) const
{
    auto definition = definition_;

    if(recurse && definition && definition->getDefinition(false)){
        definition = definition->getDefinition(true);
    }
    return definition;
}

void Node::unsetDefinition()
{
    if(definition_){
        BindDefinitionToInstance(definition_, this, false);
        definition_ = 0;
    }
}

void Node::addInstance(Node *inst)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION)){
        if(!instances_.contains(inst)){
            instances_.insert(inst);
            inst->setDefinition(this);
        }
    }
}

QSet<Node *> Node::getInstances() const
{
    return instances_;
}

void Node::removeInstance(Node *inst)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION) && instances_.remove(inst)){
        inst->unsetDefinition();
    }
}

void Node::addImplementation(Node *impl)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION)){
        if(!implementations_.contains(impl)){
            implementations_.insert(impl);
            impl->setDefinition(this);
        }
    }
}

QSet<Node *> Node::getImplementations() const
{
    return implementations_;
}

QSet<Node *> Node::getNestedDependants()
{
    QSet<Node*> dependants;

    
    for(auto dependant : getDependants()){
        dependants += dependant;
        dependants += dependant->getNestedDependants();
    }

    for(auto child : getChildren(0)){
        dependants += child;
        dependants += child->getNestedDependants();
    }

    return dependants;
}

/**
 * @brief Node::getDependants - Gets the Dependants.
 * @return
 */
QSet<Node *> Node::getDependants() const
{
    auto dependants = getImplementations();
    dependants += getInstances();
    return dependants;
}

void Node::removeImplementation(Node *impl)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION) && implementations_.remove(impl)){
        impl->unsetDefinition();
    }
}




void Node::addEdge(Edge *edge)
{
    if(!containsEdge(edge)){
        edges_.insertMulti(edge->getEdgeKind(), edge);
    }
}

void Node::removeEdge(Edge *edge)
{
    if(edge){
        edges_.remove(edge->getEdgeKind(), edge);
    }
}
void Node::parentNodeUpdated(){

    if(parent_node_){
        revalidateData();

        //Update the parent tree index
        updateTreeIndex(parent_node_->getTreeIndex());
        updateViewAspect(parent_node_->getViewAspect());
        parentSet(parent_node_);

        for(auto child : getChildren(0)){
            child->setParentNode(this, child->getBranch());
        }
    }
}

void Node::setParentNode(Node *parent, int branch)
{
    if(!parent_node_ && parent){
        //Set the persistent information once
        parent_node_ = parent;
        parent_node_kind_ = parent->getNodeKind();
        branch_ = branch;
    }
    parentNodeUpdated();
}

QString Node::toGraphML(int indent_depth, bool functional_export)
{
    QString xml;
    QTextStream stream(&xml); 

    QString tab = QString("\t").repeated(indent_depth);
    stream << tab << "<node id=\"" << getID() << "\">\n";
    
    auto data_list = getData();
    std::sort(data_list.begin(), data_list.end(), Data::SortByKey);
    for(auto data : data_list){
        stream << data->toGraphML(indent_depth + 1, functional_export);
    }

    //Children are in a <graph>
    if(getChildrenCount() > 0){
        stream << tab << "\t<graph id=\"g" << getID() << "\">\n";
        for(auto child : getChildren(0)){
            stream << child->toGraphML(indent_depth + 2, functional_export);
        }
        stream << tab << "\t</graph>\n";
    }

    stream <<  tab << "</node>\n";
    return xml;
}

QList<Node *> Node::getOrderedChildNodes()
{
    auto child_list = children_.values();
    std::sort(child_list.begin(), child_list.end(), Entity::SortByIndex);
    return child_list;
}

void Node::setChainableDefinition(){
    addInstanceKind(getNodeKind());
    addInstancesDefinitionKind(getNodeKind());
}

void Node::addInstanceKind(NODE_KIND kind){
    instance_kinds_.insert(kind);
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION, EDGE_DIRECTION::TARGET);
}

void Node::addImplKind(NODE_KIND kind){
    impl_kinds_.insert(kind);
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION, EDGE_DIRECTION::TARGET);
}

void Node::addImplsDefinitionKind(NODE_KIND kind){
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    definition_kinds_.insert(kind);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION, EDGE_DIRECTION::SOURCE);
}

void Node::addInstancesDefinitionKind(NODE_KIND kind){
    setNodeType(NODE_TYPE::INSTANCE);
    definition_kinds_.insert(kind);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION, EDGE_DIRECTION::SOURCE);
}


bool Node::isValidInstanceKind(NODE_KIND kind) const{
    return instance_kinds_.contains(kind);
}

bool Node::isValidImplKind(NODE_KIND kind) const{
    return impl_kinds_.contains(kind);
}

bool Node::isValidDefinitionKind(NODE_KIND kind) const{

    if(IsEdgeRuleActive(EdgeRule::DISALLOW_DEFINITION_CHAINING)){
        if(getNodeKind() == kind){
            return false;
        }
    }
    return definition_kinds_.contains(kind);
}

QSet<NODE_KIND> Node::getInstanceKinds() const{
    return instance_kinds_;
}

QSet<NODE_KIND> Node::getDefinitionKinds() const{
    return definition_kinds_;
}

QSet<NODE_KIND> Node::getImplKinds() const{
    return impl_kinds_;
}

void Node::LinkData(Node* source, const QString &source_key, Node* destination, const QString &destination_key, bool setup){
    auto source_data = source->getData(source_key);
    auto destination_data = destination->getData(destination_key);

    if(source_data && destination_data){
        source_data->linkData(destination_data, setup);
    }
}

void Node::BindDefinitionToInstance(Node* definition, Node* instance, bool setup){
    if(!definition || !instance){
        return;
    }
    
    auto instance_parent = instance->getParentNode();

    auto definition_kind = definition->getNodeKind();
    auto instance_kind = instance->getNodeKind();
    
    auto instance_parent_kind = instance_parent ? instance_parent->getNodeKind() : NODE_KIND::NONE;

    QMultiMap<QString, QString> bind_values;
    QMultiMap<QString, QString> copy_values;
    QSet<QString> required_instance_keys;
    bind_values.insert("key", "key");

    bind_values.insert("icon", "icon");
    bind_values.insert("icon_prefix", "icon_prefix");
    
    copy_values.insert("value", "value");

    required_instance_keys.insert("icon");
    required_instance_keys.insert("icon_prefix");

    
    bind_values.insert("is_generic_param", "is_generic_param");
    required_instance_keys.insert("is_generic_param");

    
    
    

    bool bind_index = false;
    bool bind_labels = true;
    bool bind_types = true;

    switch(instance_kind){
        case NODE_KIND::MEMBER_INSTANCE:
        case NODE_KIND::AGGREGATE_INSTANCE:{
            if(instance_parent_kind == NODE_KIND::AGGREGATE_INSTANCE || instance_parent_kind == NODE_KIND::VECTOR_INSTANCE){
                bind_index = true;
            }
            break;
        }
        default:
            break;
    }

    if(instance->isInstanceImpl()){
        switch(instance_kind){
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REQUESTER:
            case NODE_KIND::COMPONENT_INSTANCE:{
                bind_labels = false;
                break;
            }
            case NODE_KIND::AGGREGATE_INSTANCE:
            case NODE_KIND::VECTOR_INSTANCE:
            case NODE_KIND::ENUM_INSTANCE:{
                if(instance_parent_kind == NODE_KIND::AGGREGATE){
                    bind_labels = false;
                }
                break;
            };
            case NODE_KIND::CLASS_INSTANCE:{
                if(definition_kind == NODE_KIND::CLASS_INSTANCE){
                    bind_labels = true;
                }else{
                    copy_values.insert("value", "value");
                    if(definition->gotData("worker")){
                        bind_values.insert("worker", "type");
                        bind_types = false;
                    }
                }
                break;
            }
            case NODE_KIND::FUNCTION_CALL:
                if (definition->getViewAspect() == VIEW_ASPECT::WORKERS) {
                    bind_values.insert("workerID", "workerID");
                    bind_values.insert("operation", "operation");
                }
                bind_values.insert("description", "description");
                bind_values.insert("class", "class");
                bind_values.insert("is_variadic", "is_variadic");
                bind_values.insert("label", "label");
                required_instance_keys.insert("is_variadic");
                break;
            case NODE_KIND::FUNCTION:{
                bind_labels = true;
                bind_values.insert("operation", "operation");
                bind_values.insert("description", "description");
                bind_values.insert("class", "class");
                bind_values.insert("is_variadic", "is_variadic");
                
                
                required_instance_keys.insert("is_variadic");
                break;
            }
            default:
                break;
        }
    }else if(instance->isDefinition()){
        bind_labels = false;
    }
    
    auto def_got_inner_type = definition->gotData("inner_type");
    auto def_got_outer_type = definition->gotData("outer_type");

    auto inst_got_inner_type = instance->gotData("inner_type");
    auto inst_got_outer_type = instance->gotData("outer_type");


    bind_values.insert("inner_type", "inner_type");
    bind_values.insert("outer_type", "outer_type");



    if(bind_types){
        if(!def_got_inner_type && inst_got_inner_type){
            bind_values.insert("type", "inner_type");
        }else if(definition->gotData("type")){
            bind_values.insert("type", "type");
        }else if(definition->gotData("label")){
            bind_values.insert("label", "type");
        }
    }

    if(bind_labels){
        bind_values.insert("label", "label");
    }

    //Bind Index
    if(bind_index){
        bind_values.insert("index", "index");
}

    for(auto key_name : required_instance_keys){
        if(bind_values.contains(key_name)){
            auto def_data = definition->getData(key_name);
            if(def_data){
                auto key = def_data->getKey();
                if(!instance->getData(key)){
                    //Construct data
                    instance->getFactoryBroker().AttachData(instance, key, ProtectedState::PROTECTED, def_data->getValue());
                }
            }
        }
    }

    for(auto definition_key : bind_values.uniqueKeys()){
        for(auto instance_key : bind_values.values(definition_key)){
            LinkData(definition, definition_key, instance, instance_key, setup);
        }
    }

    if(setup){
        for(auto definition_key : copy_values.uniqueKeys()){
            for(auto instance_key : copy_values.values(definition_key)){
                auto def_data = definition->getData(definition_key);
                auto inst_data = instance->getData(instance_key);

                if(def_data && inst_data){
                    //Check if empty
                    if(inst_data->getValue().isNull()){
                        inst_data->setValue(def_data->getValue());
                    }
                }
            }
        }
    }
}

QList<Node*> Node::getRequiredInstanceDefinitions(){
    //Get the list of definitions, contained within valid ancestors, we should try and make instances of
    QList<Node*> adoptable_nodes;
    if(!IsEdgeRuleActive(EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS)){
        for(auto valid_ancestor : getListOfValidAncestorsForChildrenDefinitions()){
            for(auto child : valid_ancestor->getChildren(0)){
                if(child->isDefinition()){
                    adoptable_nodes << child;
                }
            }
        }
    }
    return adoptable_nodes;
}

void Node::setAcceptsEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction, bool accept){
    auto& direction_set = direction == EDGE_DIRECTION::SOURCE ? accepted_edge_kinds_as_source_ : accepted_edge_kinds_as_target_;
    
    auto size = direction_set.size();
    if(accept){
        direction_set.insert(edge_kind);
    }else{
        direction_set.remove(edge_kind);
    }
    auto post_size = direction_set.size();
    if(size != post_size){
        emit acceptedEdgeKindsChanged(this);
    }
}

bool Node::canAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const{
    auto& direction_set = direction == EDGE_DIRECTION::SOURCE ? accepted_edge_kinds_as_source_ : accepted_edge_kinds_as_target_;
    return direction_set.contains(edge_kind);
}



bool Node::canCurrentlyAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const{
    //qCritical() << "canCurrentlyAcceptEdgeKind(): " << this->toString() << (direction == EDGE_DIRECTION::SOURCE ? " SOURCE: " : " TARGET: ") << EntityFactory::getEdgeKindString(edge_kind);

    if(canAcceptEdgeKind(edge_kind, direction) == false){
        return false;
    }

    if(direction == EDGE_DIRECTION::SOURCE){
        //Source
        switch(edge_kind){
        case EDGE_KIND::DEFINITION:{
            //If we are not an instance or impl, we can never have a definition edge
            if(isInstanceImpl() == false){
                return false;
            }

            //Can have another definition if we already have one
            if(getDefinition()){
                return false;
            }
            break;
        }
        case EDGE_KIND::DATA:{
            //Call into the subclass
            if(isNodeOfType(NODE_TYPE::DATA)){
                auto data_node = (DataNode*) this;

                // A Non producer cannot have an Edge out of it
                if(!data_node->isDataProducer()){
                    ///Critical() << "4";
                    return false;
                }
                auto got_data_edge = false;

                for(auto edge : edges_.values(edge_kind)){
                    if(edge->getSource() == this){
                        got_data_edge = true;
                        break;
                    }
                }

                //Allow Linking if we don't have a data edge, or we are a MultipleData Sender
                if(got_data_edge && !data_node->isMultipleDataProducer()){
                    //qCritical() << "5";
                    return false;
                }
                return true;
            }else{
                return false;
            }
        }
        case EDGE_KIND::ASSEMBLY:{
            //Can always have more assembly edges
            //TODO: SUBCLASS FOR THAT ONE TO ONE RELATIONSHIP
            return true;
        }
        case EDGE_KIND::DEPLOYMENT:{
            if(IsEdgeRuleActive(EdgeRule::ALLOW_MULTIPLE_DEPLOYMENT_EDGES)){
                return true;
            }
        }
        case EDGE_KIND::AGGREGATE:
        case EDGE_KIND::QOS:{
            //Can only have 1 Edge is Source
            for(auto edge : edges_.values(edge_kind)){
                if(edge->getSource() == this){
                    //qCritical() << "6";
                    return false;
                }
            }
            break;
        }
        default:
            qCritical() << "NOT IMPLEMENTED";
            return false;
        }
    }else{
        //Destination
        switch(edge_kind){
        case EDGE_KIND::DEFINITION:{
            //If we are not a definition, we can never have a definition edge
            if(isDefinition() == false){
                //qCritical() << "7";
                return false;
            }

            return true;
            break;
        }
        case EDGE_KIND::DATA:{
            //Call into the subclass
            if(isNodeOfType(NODE_TYPE::DATA)){
                auto data_node = (DataNode*)this;

                // A Non producer cannot have an Edge out of it
                if(data_node->isDataReceiver() == false){
                    //qCritical() << "9"; 
                    return false;
                }

                auto got_data_edge = false;

                for(auto edge : edges_.values(edge_kind)){
                    if(edge->getDestination() == this){
                        got_data_edge = true;
                        break;
                    }
                }

                //Allow Linking if we don't have a data edge, or we are a MultipleData Sender
                if(got_data_edge && !data_node->isMultipleDataReceiver()){
                    //qCritical() << "10";
                    return false;
                }
                return true;
            }else{
                return false;
            }
        }
        case EDGE_KIND::ASSEMBLY:{
            //Can always have more assembly edges
            //TODO: SUBCLASS FOR THAT ONE TO ONE RELATIONSHIP
            return true;
        }
        case EDGE_KIND::DEPLOYMENT:
        case EDGE_KIND::AGGREGATE:
        case EDGE_KIND::QOS:{
            return true;
        }
        default:
            qCritical() << "NOT IMPLEMENTED";
            return false;
        }
    }
    return true;
}

QSet<EDGE_KIND> Node::getCurrentAcceptedEdgeKind(EDGE_DIRECTION direction) const{
    QSet<EDGE_KIND> accepted_kinds;

    auto& direction_set = direction == EDGE_DIRECTION::SOURCE ? accepted_edge_kinds_as_source_ : accepted_edge_kinds_as_target_;

    for(auto edge_kind : direction_set){
        if(canCurrentlyAcceptEdgeKind(edge_kind, direction)){
            accepted_kinds.insert(edge_kind);
        }
    }
    return accepted_kinds;
}

QSet<EDGE_KIND> Node::getAcceptedEdgeKind(EDGE_DIRECTION direction) const{
    auto& direction_set = direction == EDGE_DIRECTION::SOURCE ? accepted_edge_kinds_as_source_ : accepted_edge_kinds_as_target_;
    return direction_set;
}

NODE_KIND Node::getParentNodeKind() const{
    return parent_node_kind_;
}


QSet<Node*> Node::getListOfValidAncestorsForChildrenDefinitions(){
    QSet<Node*> valid_ancestors;

    if(getDefinition()){
        valid_ancestors << getDefinition();
    }

    return valid_ancestors;
}

void Node::setAcceptsNodeKind(NODE_KIND node_kind, bool accept){
    if(accept){
        accepted_node_kinds_.insert(node_kind);
    }else{
        accepted_node_kinds_.remove(node_kind);
    }
}

QSet<NODE_KIND> Node::getAcceptedNodeKinds() const{
    return accepted_node_kinds_;
}

bool Node::canAcceptNodeKind(NODE_KIND node_kind) const{
    return accepted_node_kinds_.contains(node_kind);
}

bool Node::canAcceptNodeKind(const Node* node) const{
    return canAcceptNodeKind(node ? node->getNodeKind() : NODE_KIND::NONE);
}

QSet<NODE_KIND> Node::getUserConstructableNodeKinds() const{
    QSet<NODE_KIND> node_kinds = getAcceptedNodeKinds();

    //If i am an instance nothing should be cosntructable
    if((isInstance() && getDefinition()) || isReadOnly()){
        node_kinds.clear();
    }
    return node_kinds;
}

QSet<Node*> Node::getParentNodesForValidDefinition(){
    QSet<Node*> ancestors;

    auto parent_node = getParentNode();
    if(parent_node){
        ancestors = parent_node->getListOfValidAncestorsForChildrenDefinitions();
    }
    return ancestors;
}

