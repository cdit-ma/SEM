#include "node.h"
#include "data.h"
#include "edge.h"
#include <algorithm>
#include <QByteArray>
#include <QDebug>
#include <QStack>
#include <QStringBuilder>

#include "../entityfactorybroker.h"
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
    broker.AttachData(this, KeyName::Kind, QVariant::String, ProtectedState::PROTECTED, broker.GetNodeKindString(node_kind));
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::UNPROTECTED, broker.GetNodeKindString(node_kind));
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED, -1);

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
        auto node_kind = child->getNodeKind();

        node_kind_count_[node_kind]++;
        new_nodes_ += child;

        child->setParentNode(this, branch_count_++);
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
    return new_nodes_.contains(child);
}

QList<Node *> Node::getChildren(int depth) const
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

QSet<Edge*> Node::getEdges(){
    return new_edges_;
}
QSet<Node*> Node::getAllChildren(){
    QSet<Node*> children;

    for(auto child : new_nodes_){
        children += child;
        children += child->getAllChildren();
    }

    return children;
}

QSet<Edge*> Node::getAllEdges(){
    auto edges = getEdges();
    
    for(auto child : getAllChildren()){
        edges += child->getEdges();
    }
    return edges;
}

int Node::getEdgeOfKindCount(EDGE_KIND edge_kind) const{
    return edge_kind_count_.value(edge_kind, 0);
}

int Node::getEdgeCount() const{
    return new_edges_.count();
}

QSet<Edge *> Node::getEdgesOfKind(EDGE_KIND edge_kind) const
{
    QSet<Edge*> edges;

    for(auto edge : new_edges_){
        if(edge->getEdgeKind() == edge_kind){
            edges += edge;
        }
    }
    return edges;
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
    QList<Node*> nodes;
    
    for(auto child : getChildren(depth)){
        if(kinds.contains(child->getNodeKind())){
            nodes.append(child);
        }
    }
    return nodes;
}

QList<Node *> Node::getChildrenOfKind(NODE_KIND kind, int depth)
{
    return getChildrenOfKinds({kind}, depth);
}

int Node::getChildrenCount() const
{
    return new_nodes_.size();
}

int Node::getChildrenOfKindCount(NODE_KIND kind) const{
    return node_kind_count_.value(kind, 0);
}

void Node::childRemoved(Node* child){
    IndexKey::RevalidateChildrenIndex(this);
}


bool Node::removeChild(Node *child)
{
    if(containsChild(child)){
        auto node_kind = child->getNodeKind();
        
        node_kind_count_[node_kind]--;
        new_nodes_ -= child;
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
    for(auto edge : getEdgesOfKind(edge_kind)){
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
    return new_edges_.contains(edge);
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
    getNestedDependants_(dependants);
    return dependants;
}

void Node::getNestedDependants_(QSet<Node*>& dependants){
    //All our children are dependants
    for(auto child : getOrderedChildNodes()){
        if(!dependants.contains(child)){
            dependants += child;
            child->getNestedDependants_(dependants);
        }
    }

    for(auto dependant : getDependants()){
        if(!dependants.contains(dependant)){
            dependants += dependant;
            dependant->getNestedDependants_(dependants);
        }
    }
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
        new_edges_ += edge;
        edge_kind_count_[edge->getEdgeKind()] ++;
    }
}

void Node::removeEdge(Edge *edge)
{
    if(edge){
        new_edges_ -= edge;
        edge_kind_count_[edge->getEdgeKind()] --;
    }
}
void Node::AddUUID(){
    if(parent_node_){
        auto parent_uuid = parent_node_->getData(KeyName::UUID);
        if(parent_uuid){
            auto uuid_key = parent_uuid->getKey();
            if(!gotData(uuid_key)){
                //Construct a new piece of data
                getFactoryBroker().AttachData(this, uuid_key, ProtectedState::PROTECTED);
            }
        }
    }
}

void Node::parentNodeUpdated(){

    if(parent_node_){
        revalidateData();

        AddUUID();
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


QList<Node *> Node::getOrderedChildNodes() const
{
    auto child_list = new_nodes_.toList();
    auto index_key = getKey(KeyName::Index);
    std::sort(child_list.begin(), child_list.end(), [index_key](const Node* a, const Node* b){
        auto a_ind = a->getDataValue(index_key).toInt();
        auto b_ind = b->getDataValue(index_key).toInt();
        return a_ind < b_ind;
    });
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

void Node::BindDefinitionToInstance(Node* definition, Node* instance, bool setup){
    if(!definition || !instance){
        return;
    }
    
    auto instance_parent = instance->getParentNode();

    auto definition_kind = definition->getNodeKind();
    auto instance_kind = instance->getNodeKind();
    
    auto instance_parent_kind = instance_parent ? instance_parent->getNodeKind() : NODE_KIND::NONE;

    auto definition_aspect = definition->getViewAspect();
    
    QHash<QString, QSet<QString> > bind_values;
    QHash<QString, QSet<QString> > copy_values;
    QSet<QString> required_instance_keys;

    bind_values[KeyName::Key] += KeyName::Key;
    bind_values[KeyName::Icon] += KeyName::Icon;
    bind_values[KeyName::IconPrefix] += KeyName::IconPrefix;

    copy_values[KeyName::Value] += KeyName::Value;

    required_instance_keys += KeyName::Icon;
    required_instance_keys += KeyName::IconPrefix;


    bind_values[KeyName::IsGenericParam] += KeyName::IsGenericParam;
    bind_values[KeyName::IsGenericParamSrc] += KeyName::IsGenericParamSrc;
    bind_values[KeyName::IsOptionalParam] += KeyName::IsOptionalParam;
    bind_values[KeyName::CppVarType] += KeyName::CppVarType;
    

    required_instance_keys += KeyName::IsGenericParam;
    required_instance_keys += KeyName::IsGenericParamSrc;
    required_instance_keys += KeyName::IsOptionalParam;
    required_instance_keys += KeyName::CppVarType;

    bool bind_index = false;
    bool bind_labels = true;
    bool bind_types = true;

    switch(instance_kind){
        case NODE_KIND::CLASS_INST:{
            bind_values[KeyName::IsWorker] += KeyName::IsWorker;
            break;
        }
        case NODE_KIND::MEMBER_INST:
        case NODE_KIND::VECTOR_INST:
        case NODE_KIND::AGGREGATE_INST:{
            static const QSet<NODE_KIND> bind_index_kinds{NODE_KIND::AGGREGATE_INST, NODE_KIND::VECTOR_INST, NODE_KIND::INPUT_PARAMETER_GROUP_INST};
            if(bind_index_kinds.contains(instance_parent_kind)){
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
            case NODE_KIND::COMPONENT_INST:{
                bind_labels = false;
                break;
            }
            case NODE_KIND::MEMBER_INST:
            case NODE_KIND::AGGREGATE_INST:
            case NODE_KIND::VECTOR_INST:
            case NODE_KIND::ENUM_INST:{
                static const QSet<NODE_KIND> unbound_labels{NODE_KIND::AGGREGATE, NODE_KIND::INPUT_PARAMETER_GROUP, NODE_KIND::RETURN_PARAMETER_GROUP};
                if(unbound_labels.contains(instance_parent_kind)){
                    bind_labels = false;
                }

                if (definition->getViewAspect() == VIEW_ASPECT::WORKERS) {
                    bind_values[KeyName::Description] += KeyName::Description;
                    required_instance_keys.insert(KeyName::Description);
                }
                break;
            };
            case NODE_KIND::CLASS_INST:{
                bind_values[KeyName::Version] += KeyName::Version;
                required_instance_keys.insert(KeyName::Version);

                if(definition_kind == NODE_KIND::CLASS_INST){
                    bind_labels = true;
                }else{
                    copy_values[KeyName::Value] += KeyName::Value;
                    if(definition->gotData(KeyName::Worker)){
                        bind_values[KeyName::Worker] += KeyName::Type;
                        bind_types = false;
                    }
                }
                break;
            }
            case NODE_KIND::CALLBACK_FNC_INST:
            case NODE_KIND::FUNCTION_CALL: 
                if (definition->getViewAspect() == VIEW_ASPECT::WORKERS) {
                    bind_values[KeyName::WorkerID] += KeyName::WorkerID;
                    bind_values[KeyName::Operation] += KeyName::Operation;
                    bind_values[KeyName::Version] += KeyName::Version;
                }
                bind_values[KeyName::Description] += KeyName::Description;
                bind_values[KeyName::Class] += KeyName::Class;
                bind_values[KeyName::IsVariadic] += KeyName::IsVariadic;
                bind_values[KeyName::Label] += KeyName::Label;
                required_instance_keys.insert(KeyName::IsVariadic);
                required_instance_keys.insert(KeyName::Description);
                break;
            case NODE_KIND::CALLBACK_FNC:
            case NODE_KIND::FUNCTION:{
                bind_labels = true;
                bind_values[KeyName::Operation] += KeyName::Operation;
                bind_values[KeyName::Description] += KeyName::Description;
                bind_values[KeyName::Class] += KeyName::Class;
                bind_values[KeyName::IsVariadic] += KeyName::IsVariadic;
                
                
                required_instance_keys.insert(KeyName::IsVariadic);
                required_instance_keys.insert(KeyName::Description);
                break;
            }
            default:
                break;
        }
    }else if(instance->isDefinition()){
        bind_labels = false;
    }
    
    auto def_got_inner_type = definition->gotData(KeyName::InnerType);

    auto inst_got_inner_type = instance->gotData(KeyName::InnerType);


    bind_values[KeyName::InnerType] += KeyName::InnerType;
    bind_values[KeyName::OuterType] += KeyName::OuterType;



    if(bind_types){
        if(!def_got_inner_type && inst_got_inner_type){
            bind_values[KeyName::Type] += KeyName::InnerType;
        }else if(definition->gotData(KeyName::Type)){
            bind_values[KeyName::Type] += KeyName::Type;
        }else if(definition->gotData(KeyName::Label)){
            bind_values[KeyName::Label] += KeyName::Type;
        }
    }

    if(bind_labels){
        bind_values[KeyName::Label] += KeyName::Label;
    }

    //Bind Index
    if(bind_index){
        bind_values[KeyName::Index] += KeyName::Index;
    }
    bind_values[KeyName::ColumnCount] += KeyName::ColumnCount;

    for(const auto& key_name : required_instance_keys){
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

    for(const auto& definition_key : bind_values.keys()){
        for(const auto& instance_key : bind_values[definition_key]){
            Data::LinkData(definition, definition_key, instance, instance_key, setup);
        }
    }

    if(setup){
        for(const auto& definition_key : copy_values.keys()){
            for(const auto& instance_key : copy_values[definition_key]){
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

                for(auto edge : getEdgesOfKind(edge_kind)){
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
            for(auto edge : getEdgesOfKind(edge_kind)){
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

                for(auto edge : getEdgesOfKind(edge_kind)){
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
    const auto is_valid_instance = isInstance() && getDefinition();
    if(is_valid_instance && !IsEdgeRuleActive(EdgeRule::ALWAYS_ALLOW_ADOPTION_AS_INSTANCE)){
        node_kinds.clear();
    }

    if(isReadOnly()){
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

