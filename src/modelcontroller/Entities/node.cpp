#include "node.h"
#include "data.h"
#include "edge.h"
#include <algorithm>
#include <QDebug>
#include <QCryptographicHash>
#include <QStringBuilder>
#include <QByteArray>
#include <QStack>
#include "../entityfactory.h"

#include "InterfaceDefinitions/datanode.h"

#include "../nodekinds.h"
#include "../edgekinds.h"
#include "Keys/indexkey.h"
#include "Keys/typekey.h"

Node::Node(NODE_KIND node_kind):Entity(GRAPHML_KIND::NODE)
{
    nodeKind = node_kind;
    parentNode = 0;
    childCount = 0;
    definition = 0;
    aspect = VIEW_ASPECT::NONE;

    
    SetEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY);
    SetEdgeRuleActive(EdgeRule::REQUIRE_NO_DEFINITION);
}


void Node::RegisterNodeKind(EntityFactory* factory, NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor){
    if(factory){
        factory->RegisterNodeKind(kind, kind_string, constructor);
    }
}

void Node::RegisterDefaultData(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected, QVariant value){
    if(factory){
        factory->RegisterDefaultData(kind, key_name, type, is_protected, value);
    }
}

void Node::RegisterValidDataValues(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values){
    if(factory){
        factory->RegisterValidDataValues(kind, key_name, type, values);
    }
}


Node::Node(EntityFactory* factory, NODE_KIND kind, QString kind_string):Entity(GRAPHML_KIND::NODE){
    //Register kind, label and index as required data
    RegisterDefaultData(factory, kind, "kind", QVariant::String, true, kind_string);
    RegisterDefaultData(factory, kind, "label", QVariant::String, false, kind_string);
    RegisterDefaultData(factory, kind, "index", QVariant::Int, true, -1);
    
}


int Node::getDepth() const{
    return treeIndex.size();
}

QList<int> Node::getTreeIndex()
{
    return treeIndex;
}

QString Node::getTreeIndexAlpha()
{
    return treeIndexString;
}

NODE_KIND Node::getNodeKind() const
{
    return nodeKind;
}

bool Node::isAttached() const
{
    return treeIndex.length() > 0;
}


Node::~Node()
{
    if(parentNode){
        parentNode->removeChild(this);
    }
}

void Node::SetEdgeRuleActive(EdgeRule rule, bool active){
    if(active){
        active_edge_rules.insert(rule);
    }else{
        active_edge_rules.remove(rule);
    }
}


bool Node::IsEdgeRuleActive(EdgeRule rule){
    return active_edge_rules.contains(rule);
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


            if(!parent_node->isInstanceImpl()){
                //Only allow edges to things which don't have a definition
                if(dst->getDefinition()){
                    return false;
                }
            }


        if(IsEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY)){
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
            }
            
            /*
            When this is contained within and instance or impl
            we should check that the dst we are connecting to (AKA Using as this's definition)
            is contained within our parent's Definition/Implementations
            */
            if(parentNode && parentNode->isInstanceImpl()){
                auto valid_ancestors = parentNode->getListOfValidAncestorsForChildrenDefinitions();
                
                bool is_descendant = false;
                for(auto ancestor : valid_ancestors){
                    if(dst->getParentNode() == ancestor){
                        is_descendant = true;
                        break;
                    }
                }
                
                if(!is_descendant && valid_ancestors.size() > 0){
                    return false;
                }
            }
        }

       
        /*if(IsEdgeRuleActive(EdgeRule::REQUIRE_NO_DEFINITION)){
            /*
            When this isn't contained within and instance or impl
            we should check that the dst we are connecting to (AKA Using as this's definition)
            doesn't have a definition set
            */
        //}*

        if(indirectlyConnectedTo(dst)){
            return false;
        }

        break;
    }
    case EDGE_KIND::DATA:{
        break;
    }
    case EDGE_KIND::QOS:{
        if(!dst->isNodeOfType(NODE_TYPE::QOS_PROFILE)){
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

    //Needed for cool edges
    if(!dst->isInModel()){
        //Don't allow edges to outside the model.
        return false;
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
    return types.contains(type);
}


void Node::setNodeType(NODE_TYPE type)
{
    types.insert(type);
}

void Node::removeNodeType(NODE_TYPE type)
{
    types.remove(type);
}



int Node::getDepthFromAspect()
{
    int depth = 0;

    Node* parent = this;
    while(parent){
        if(parent->isAspect()){
            return depth;
        }
        parent = parent->getParentNode();
        depth ++;
    }
    return -1;
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
    treeIndex.append(index);
    treeIndexString = QChar('A' + index);
}

VIEW_ASPECT Node::getViewAspect() const
{
    return aspect;
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


Node *Node::getParentNode(int depth)
{

    if(depth < 0){
        return 0;
    }
    if(depth == 0){
        return this;
    }
    if(depth == 1){
        return parentNode;
    }
    Node* node = this;
    while(depth >= 1){
        if(node){
            node = node->getParentNode();
        }
        depth --;
    }
    return node;
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

    /*if(!canAcceptNodeKind(node)){
        return false;
    }*/

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
        qCritical() << "CANNOT ADOPT SOMETHING WITH A DEFINITION";
        return false;
    }

    return true;
}

bool Node::compareData(Node *node, QString keyName)
{
    if(node){
        Data* d1 = getData(keyName);
        Data* d2 = node->getData(keyName);
        if(d1 && d2){
            return d1->compare(d2);
        }
    }
    return false;

}

bool Node::compareData(Node *node, QStringList keys)
{
    foreach(QString key, keys){
        if(!compareData(node, key)){
            return false;
        }
    }
    return true;
}

void Node::setViewAspect(VIEW_ASPECT aspect)
{
    this->aspect = aspect;
}

bool Node::addChild(Node *child)
{
    if(child && !containsChild(child) && canAdoptChild(child)){
        children << child;
        child->setParentNode(this, childCount++);

        if(gotData("uuid")){
            auto data = getData("uuid");
            auto key = data->getKey();
            if(!child->gotData("uuid")){
                auto child_data = new Data(key);
                child->addData(child_data);
            }
        }
        childAdded(child);
        return true;
    }
    return false;
}

bool Node::setAsRoot(int root_index)
{
    if(!parentNode && treeIndex.length() == 0){
        setTop(root_index);
        return true;
    }
    return false;
}

QString Node::getNodeKindStr()
{
    return EntityFactory::getNodeKindString(nodeKind);
}

bool Node::indirectlyConnectedTo(Node *node)
{
    Node* parent = getParentNode();

    QStack<Node*> items;
    items.append(node);
    while(!items.isEmpty()){
        Node* item = items.takeFirst();

        if(item == parent){
            return true;
        }
        Node* itemDef = item->getDefinition(true);
        if(itemDef){
            if(itemDef == parent){
                return true;
            }

            if(itemDef == parent->getDefinition(true)){
                return true;
            }
        }

        foreach(Node* node, item->getChildren(0)){
            items.append(node);
        }
    }

    return false;
}

bool Node::containsChild(Node *child)
{
    return children.contains(child);
}

QList<Node *> Node::getChildren(int depth)
{
    QList<Node *> childList = getOrderedChildNodes();

    //While we still have Children, Recurse
    if(depth != 0){
        //Add children's children.
        foreach(Node* child, getChildren(0)){
            childList += child->getChildren(depth - 1);
        }
    }

    return childList;
}

QList<int> Node::getChildrenIDs(int depth)
{
    QList<int> childIDs;
    foreach(Node* child, getChildren(depth)){
        if(child){
            childIDs.append(child->getID());
        }
    }
    return childIDs;
}


QList<Edge*> Node::getAllEdges(){
    return getEdges(-1, EDGE_KIND::NONE);
}

bool Node::gotEdgeKind(EDGE_KIND kind){
    return edges.contains(kind);
}

QList<Edge *> Node::getEdges(int depth, EDGE_KIND edgeKind)
{
    QList<Edge*> edgesToMap;

    if(edgeKind == EDGE_KIND::NONE){
        edgesToMap += edges.values();
    }else{
        edgesToMap += edges.values(edgeKind);
    }

    //While we still have Children, Recurse
    if(depth != 0){
        //Add children's children.
        foreach(Node* child, getChildren(0)){
            foreach(Edge* edge, child->getEdges(depth - 1, edgeKind)){
                if(!edgesToMap.contains(edge)){
                    edgesToMap += edge;
                }
            }
        }
    }

    QMap<EDGE_KIND, Edge *> edgeMap;
    foreach(Edge* edge, edgesToMap){
        edgeMap.insertMulti(edge->getEdgeKind(), edge);
    }

    return edgeMap.values();
}


Node *Node::getFirstChild()
{
    auto list = getOrderedChildNodes();
    if(list.length() > 0){
        return list.first();
    }
    return 0;
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

QList<Node *> Node::getChildrenOfKind(QString kindStr, int depth)
{
    QList<Node*> returnableList;
    foreach(Node* childNode, getChildren(depth)){
        if(childNode->getDataValue("kind") == kindStr){
            returnableList.append(childNode);
        }
    }
    return returnableList;
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

QList<int> Node::getEdgeIDs(EDGE_KIND edgeClass)
{
    QList<int> returnable;
    foreach(Edge* edge, getEdges(0, edgeClass)){
        returnable += edge->getID();
    }
    return returnable;
}



/*
Node *Node::getChild(int position)
{
    if(orderedChildren.contains(position)){
        return orderedChildren[position];
    }
    return 0;
}

*/
int Node::childrenCount()
{
    return children.size();
}

bool Node::hasChildren()
{
    return !children.isEmpty();
}

bool Node::hasEdges()
{
    return !edges.isEmpty();
}

void Node::childRemoved(Node* child){
    IndexKey::RevalidateChildrenIndex(this);
}


bool Node::removeChild(Node *child)
{
    int removeCount = 0;

    if(child){
        removeCount = children.removeAll(child);
    }

    if(removeCount > 0){
        childRemoved(child);
        //Recontiguate 
        return true;
    }
    return false;
}

void Node::addValidValue(QString key_name, QVariant value){
    if(!valid_values_.contains(key_name, value)){
        valid_values_.insert(key_name, value);
    }
}
void Node::addValidValues(QString key_name, QList<QVariant> values){
    foreach(auto value, values){
        addValidValue(key_name, value);
    }
}

QStringList Node::getValidValueKeys(){
    return valid_values_.uniqueKeys();
}

QList<QVariant> Node::getValidValues(QString key_name){
    return valid_values_.values(key_name);
}


bool Node::ancestorOf(Node *node)
{
    auto b_tree_index = node->getTreeIndex();

    //Not an ancestor of ourselves
    if(this == node){
        return true;
    }

    //If either of tree's aren't setup, we can't be an ancestor
    if(treeIndex.size() == 0 || b_tree_index.size() == 0){
        return false;
    }

    //If this node is shallower than the comparison, has to be false
    if(this->treeIndex.size() > b_tree_index.size()){
        return false;
    }

    for(int i = 0; i < treeIndex.size(); i++){
        if(treeIndex.at(i) != b_tree_index.at(i)){
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

    return getAllEdges().contains(edge);
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
    if(treeIndex.size() == 0 || b_tree_index.size() == 0){
        return false;
    }

    if(this->treeIndex.size() < b_tree_index.size()){
        return false;
    }

    for(int i = 0; i < b_tree_index.size(); i++){
        if(treeIndex.at(i) != b_tree_index.at(i)){
            return false;
        }
    }
    return true;
}



Edge* Node::getEdgeTo(Node *node, EDGE_KIND edgeKind)
{
    foreach(Edge* edge, getEdges(0, edgeKind)){
        if(edge->isConnected(node)){
            return edge;
        }
    }
    return 0;
}


bool Node::gotEdgeTo(Node *node, EDGE_KIND edgeKind)
{
    return getEdgeTo(node, edgeKind) != 0;
}



bool Node::containsEdge(Edge *edge)
{
    if(edge){
        return edges.contains(edge->getEdgeKind(), edge);
    }
    return false;
}



QList<Key *> Node::getKeys(int depth)
{
    QList<Key*> allKeys;
    QList<Data*> allData;

    allData += getData();
    foreach(Entity* entity, getChildren(depth)){
        allData += entity->getData();
    }
    foreach(Entity* entity, getEdges(depth, EDGE_KIND::NONE)){
        allData += entity->getData();
    }

    foreach(Data* data, allData){
        Key* key = data->getKey();
        if(!allKeys.contains(key)){
            allKeys += key;
        }
    }

    return allKeys;
}

QString Node::toMD5Hash()
{
    QStringList includedKeys;
    includedKeys << "label" << "kind" << "type" << "key";
    QString returnable;
    foreach(Data* data, getData()){
        if(includedKeys.contains(data->getKeyName())){
            returnable += data->getKeyName() + "=" + data->getValue().toString() + ";";
        }
    }
    QString hash = QCryptographicHash::hash(returnable.toUtf8(), QCryptographicHash::Md5).toHex();
    return hash;
}

bool Node::isInModel()
{
    if(treeIndex.size() > 0){
        if(treeIndex.first() == 0){
            return true;
        }
    }
    return false;
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
    return types;
}

void Node::setDefinition(Node *def)
{
    if(isImpl() || isInstance()){
        definition = def;
        BindDefinitionToInstance(definition, this, true);

        //If have a definition, 
        bool can_have_definition_edge = false;

        if(definition){
            if(isInstance()){
                can_have_definition_edge = true;
            }
        }else{
            can_have_definition_edge = true;
        }

        definitionSet(def);
    }
}

Node *Node::getDefinition(bool recurse) const
{
    Node* def = definition;


    if(recurse && def && def->getDefinition(false)){
        def = def->getDefinition(true);
    }
    return def;
}

void Node::unsetDefinition()
{
    if(definition){

        BindDefinitionToInstance(definition, this, false);
        definition = 0;
    }
}

void Node::addInstance(Node *inst)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION)){
        if(!instances.contains(inst)){
            instances.append(inst);
            inst->setDefinition(this);
        }
    }
}

QList<Node *> Node::getInstances() const
{
    return instances;
}

void Node::removeInstance(Node *inst)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION)){
        instances.removeAll(inst);
        instances.removeAll(inst);
        inst->unsetDefinition();
    }
}

void Node::addImplementation(Node *impl)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION)){
        if(!implementations.contains(impl)){
            implementations.push_back(impl);
            impl->setDefinition(this);
        }
    }
}

QList<Node *> Node::getImplementations() const
{
    return implementations;
}

QSet<Node *> Node::getNestedDependants()
{
    QSet<Node*> nodes;

    //Get all our instances/Impls, get there 
    for(auto dependant : getDependants()){
        nodes.insert(dependant);
        for(auto d_child : dependant->getNestedDependants()){
            nodes.insert(d_child);
        }
    }

    for(auto child : getChildren(0)){
        nodes.insert(child);
        for(auto dependant : child->getNestedDependants()){
            nodes.insert(dependant);
        }
    }

    return nodes;//.toList();
}

/**
 * @brief Node::getDependants - Gets the Dependants.
 * @return
 */
QSet<Node *> Node::getDependants() const
{
    QSet<Node*> nodes;
    for(auto i : implementations){
        nodes.insert(i);
    }
    for(auto i : instances){
        nodes.insert(i);
    }
   
    return nodes;
}

void Node::removeImplementation(Node *impl)
{
    if(isNodeOfType(NODE_TYPE::DEFINITION)){
        implementations.removeAll(impl);
        impl->unsetDefinition();
    }

}




void Node::addEdge(Edge *edge)
{
    if(!containsEdge(edge)){
        edges.insertMulti(edge->getEdgeKind(), edge);
    }
}

void Node::removeEdge(Edge *edge)
{
    if(edge){
        edges.remove(edge->getEdgeKind(), edge);
    }
}

void Node::setParentNode(Node *parent, int index)
{
    if(parent){
        treeIndex = parent->getTreeIndex();
        treeIndex.append(index);

        treeIndexString = parent->getTreeIndexAlpha() % QChar('A' + index);
        parentNode = parent;

        parent_node_kind = parent->getNodeKind();
        //Set the view Aspect.
        setViewAspect(parent->getViewAspect());

        for(auto data : getData()){
            data->revalidateData();
        }

        parentSet(parent);
    }else{
        setViewAspect(VIEW_ASPECT::NONE);
        parent_node_kind = NODE_KIND::NONE;
    }
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
    if(hasChildren()){
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
    std::sort(children.begin(), children.end(), [](const Node* a, const Node* b){
        auto a_ind = a->getDataValue("index").toInt();
        auto b_ind = b->getDataValue("index").toInt();
        return a_ind < b_ind;
        });
    return children;
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

void LinkData(Node* source, const QString &source_key, Node* destination, const QString &destination_key, bool setup){
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
    //qCritical() << "BINDING DEFINITION: " << definition << " TO " << instance << " " << (setup ? "LINK" : "UNLINK");
    auto instance_parent = instance->getParentNode();

    auto definition_kind = definition->getNodeKind();
    auto instance_kind = instance->getNodeKind();
    
    auto instance_parent_kind = instance_parent ? instance_parent->getNodeKind() : NODE_KIND::NONE;

    QMultiMap<QString, QString> bind_values;
    bind_values.insert("key", "key");

    bind_values.insert("icon", "icon");
    bind_values.insert("icon_prefix", "icon_prefix");
    bind_values.insert("worker", "worker");
    

    bool bind_index = false;
    bool bind_labels = true;
    bool bind_types = true;
    bool copy_labels = false;

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
                    bind_labels = false;
                    copy_labels = true;
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
                break;
            case NODE_KIND::FUNCTION:{
                bind_labels = true;
                bind_values.insert("operation", "operation");
                bind_values.insert("description", "description");
                bind_values.insert("class", "class");
                break;
            }
            default:
                break;
        }
    }else if(instance->isDefinition()){
        bind_labels = false;
    }

    if(bind_types){
        if(definition->gotData("type")){
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


    auto def_icon_prefix = definition->getData("icon_prefix");
    auto def_icon = definition->getData("icon");
    if(def_icon && def_icon_prefix){
        auto key_icon_prefix = def_icon_prefix->getKey();
        auto key_icon = def_icon->getKey();
        auto factory = instance->getFactory();

        //Construct Icon Prefix if we don't have one already
        if(!instance->gotData(key_icon_prefix)){
            auto data_icon_prefix = factory->CreateData(key_icon_prefix, def_icon_prefix->getValue());
            instance->addData(data_icon_prefix);
        }
        //Construct Icon if we don't have one already
        if(!instance->gotData(key_icon)){
            auto data_icon = factory->CreateData(key_icon, def_icon->getValue());
            instance->addData(data_icon);
        }
    }

    for(auto definition_key : bind_values.uniqueKeys()){
        for(auto instance_key : bind_values.values(definition_key)){
            LinkData(definition, definition_key, instance, instance_key, setup);
        }
    }

    if(copy_labels){
        auto def_label = definition->getDataValue("label");
        instance->setDataValue("label", def_label);
    }
}

void Node::BindDataRelationship(Node* source, Node* destination, bool setup){
    if(source && destination && source->isNodeOfType(NODE_TYPE::DATA) && destination->isNodeOfType(NODE_TYPE::DATA)){
        auto source_parent = source->getParentNode();
        auto destination_parent = destination->getParentNode();

        if(destination_parent){
            if(destination_parent->getNodeKind() == NODE_KIND::FUNCTION_CALL){
                auto worker_name = destination_parent->getDataValue("worker").toString();
                auto parameter_label = destination->getDataValue("label").toString();

                if(worker_name == "OpenCL_Worker" || worker_name == "Vector_Operations"){
                    for(auto param : destination_parent->getChildren(0)){
                        if(param->isNodeOfType(NODE_TYPE::PARAMETER)){
                            //Check if we are using generic params
                            auto is_generic_param = param->getDataValue("is_generic_param").toBool();
                            if(is_generic_param){
                                LinkData(source, "inner_type", param, "inner_type", setup);
                                if(!setup){
                                    param->setDataValue("inner_type", "");
                                }
                            }
                        }
                    }
                }
            }
            if(destination_parent->getNodeKind() == NODE_KIND::SETTER){
                for(auto param : destination_parent->getChildren(0)){
                    if(param->isNodeOfType(NODE_TYPE::PARAMETER)){
                        LinkData(source, "inner_type", param, "inner_type", setup);
                        LinkData(source, "outer_type", param, "outer_type", setup);
                    }
                }
            }
            
        }
        auto bind_source = source;
        auto source_key = "type";

        //Data bind to the Variable, instead of the Member
        if(source_parent && source_parent->getNodeKind() == NODE_KIND::VARIABLE){
            bind_source = source_parent;
        }

        //BIND LABEL
        QSet<NODE_KIND> bind_labels = {NODE_KIND::VARIABLE, NODE_KIND::ATTRIBUTE_IMPL, NODE_KIND::ENUM_MEMBER, NODE_KIND::DEPLOYMENT_ATTRIBUTE};

        if(bind_labels.contains(bind_source->getNodeKind())){
            source_key = "label";
        }

        LinkData(bind_source, source_key, destination, "value", setup);
        TypeKey::BindInnerAndOuterTypes(bind_source, destination, setup);
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

bool Node::canConstructChildren() const{
    if(isInstance() && getDefinition()){
        return false;
    }

    return true;
}

void Node::setAcceptsEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction, bool accept){
    auto& direction_set = direction == EDGE_DIRECTION::SOURCE ? accepted_edge_kinds_as_source_ : accepted_edge_kinds_as_target_;
    
    if(accept){
        direction_set.insert(edge_kind);
    }else{
        direction_set.remove(edge_kind);
    }

    auto factory = getFactory();
    if(factory){
        factory->acceptedEdgeKindsChanged(this);
    }
}

bool Node::canAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const{
    auto& direction_set = direction == EDGE_DIRECTION::SOURCE ? accepted_edge_kinds_as_source_ : accepted_edge_kinds_as_target_;
    return direction_set.contains(edge_kind);
}



bool Node::canCurrentlyAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const{
    //qCritical() << "canCurrentlyAcceptEdgeKind(): " << this->toString() << (direction == EDGE_DIRECTION::SOURCE ? " SOURCE: " : " TARGET: ") << EntityFactory::getEdgeKindString(edge_kind);

    if(canAcceptEdgeKind(edge_kind, direction) == false){
        //qCritical() << "1";
        return false;
    }

    if(direction == EDGE_DIRECTION::SOURCE){
        //Source
        switch(edge_kind){
        case EDGE_KIND::DEFINITION:{
            //If we are not an instance or impl, we can never have a definition edge
            if(isInstanceImpl() == false){
                //qCritical() << "2";
                return false;
            }

            //Can have another definition if we already have one
            if(getDefinition()){
                //qCritical() << "3";
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
                    //qCritical() << "4";
                    return false;
                }
                auto got_data_edge = false;

                for(auto edge : edges.values(edge_kind)){
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
        case EDGE_KIND::DEPLOYMENT:
        case EDGE_KIND::AGGREGATE:
        case EDGE_KIND::QOS:{
            //Can only have 1 Edge is Source
            for(auto edge : edges.values(edge_kind)){
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

                for(auto edge : edges.values(edge_kind)){
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
    return parent_node_kind;
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
    if(isInstance() || isReadOnly()){
        node_kinds.clear();
    }
    return node_kinds;
}