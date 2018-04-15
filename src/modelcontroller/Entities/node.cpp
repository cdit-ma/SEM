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

    definition_kind_ = NODE_KIND::NONE;
    instance_kind_ = NODE_KIND::NONE;
    impl_kind_ = NODE_KIND::NONE;
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

/**
 * @brief Node::addValidEdgeType Add's an Edge class as a type of edge this node should check in canConnect()
 * @param validEdge
 */
bool Node::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!requiresEdgeKind(edgeKind)){
        return false;
    }
    
    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{

        //This must be an Instance/Impl Node Type
        if(!(isInstanceImpl())){
            return false;
        }

        //Node cannot already have a Definition.
        if(getDefinition()){
            return false;
        }

  
        //Node must be a Definition Node Type.
        if(!dst->isNodeOfType(NODE_TYPE::DEFINITION)){
            return false;
        }
       

        if(parentNode){
            if(parentNode->isInstanceImpl()){
                QList<Node*> valid_ancestors;
                auto parent_definition = parentNode->getDefinition();
                if(parent_definition){
                    valid_ancestors << parent_definition;
                    valid_ancestors << parent_definition->getImplementations();
                }
                
                bool is_descendant = false;
                for(auto ancestor : valid_ancestors){
                    if(ancestor && ancestor->isAncestorOf(dst)){
                        is_descendant = true;
                        break;
                    }
                }

                if(!is_descendant && valid_ancestors.size()){
                    //An Entity cannot be connected to It's definition if it's not contained in the parents definition Entity.
                    return false;
                }
            }else{
                if(dst->getDefinition()){
                    return false;
                }
            }
        }

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

    if(gotEdgeTo(dst, edgeKind)){
        //Don't allow multi edges of the same kind.
        return false;
    }
    return true;
}

bool Node::isNodeOfType(NODE_TYPE type) const
{
    return types.contains(type);
}

bool Node::acceptsEdgeKind(EDGE_KIND edge_kind) const
{
    return accepted_edge_kinds_.contains(edge_kind);
}

QSet<EDGE_KIND> Node::getRequiredEdgeKinds() const{
    QSet<EDGE_KIND> required_edge_kinds;
    for(auto edge_kind: accepted_edge_kinds_){
        if(requiresEdgeKind(edge_kind)){
            required_edge_kinds.insert(edge_kind);
        }
    }
    return required_edge_kinds;
}


bool Node::requiresEdgeKind(EDGE_KIND edgeKind) const
{
    if(acceptsEdgeKind(edgeKind)){
        switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            if(definition){
                return false;
            }

            if(isDefinition() && !isInstanceImpl()){
               return false;
            }
            break;
        }
        case EDGE_KIND::ASSEMBLY:
            //Can always have more assembly edges
            return true;
        case EDGE_KIND::DEPLOYMENT:
            return true;
        case EDGE_KIND::AGGREGATE:
        case EDGE_KIND::QOS:{
            for(auto edge : edges.values(edgeKind)){
                if(edge->getSource() == this){
                    return false;
                }
            }
            break;
        }
        case EDGE_KIND::DATA:{
            break;
        }
        default:
            return false;
        }
        return true;
    }
    return false;
}

QSet<EDGE_KIND> Node::getValidEdgeKinds() const{
    return accepted_edge_kinds_;
}

void Node::setNodeType(NODE_TYPE type)
{
    types.insert(type);
}

void Node::removeNodeType(NODE_TYPE type)
{
    types.remove(type);
}

void Node::setAcceptsEdgeKind(EDGE_KIND edge_kind, bool accept)
{
    if(accept){
        accepted_edge_kinds_.insert(edge_kind);
    }else{
        accepted_edge_kinds_.remove(edge_kind);
    }
    auto factory = getFactory();
    if(factory){
        factory->acceptedEdgeKindsChanged(this);
    }
}

void Node::removeEdgeKind(EDGE_KIND edge_kind)
{
    setAcceptsEdgeKind(edge_kind, false);
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

QList<Node *> Node::getChildrenOfKind(NODE_KIND kind, int depth)
{
    QList<Node*> returnableList;
    foreach(Node* childNode, getChildren(depth)){
        if(childNode->getNodeKind() == kind){
            returnableList.append(childNode);
        }
    }
    return returnableList;
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
        setAcceptsEdgeKind(EDGE_KIND::DEFINITION, !definition);
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
    if(isImpl()){
        //Get My Definitions Instances
        auto definition = getDefinition(true);
        if(definition){
            for(auto d : definition->getInstances()){
                nodes.insert(d);
            }
        }
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

QSet<EDGE_KIND> Node::getAcceptedEdgeKinds() const{
    return accepted_edge_kinds_;
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

        //Set the view Aspect.
        setViewAspect(parent->getViewAspect());

        for(auto data : getData()){
            data->revalidateData();
        }

        parentSet(parent);
    }else{
        setViewAspect(VIEW_ASPECT::NONE);
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

void Node::setInstanceKind(NODE_KIND kind){
    instance_kind_ = kind;
}

void Node::setDefinitionKind(NODE_KIND kind){
    definition_kind_ = kind;
}
void Node::setImplKind(NODE_KIND kind){
    impl_kind_ = kind;
}

NODE_KIND Node::getInstanceKind() const{
    return instance_kind_;
}

NODE_KIND Node::getDefinitionKind() const{
    return definition_kind_;
}

NODE_KIND Node::getImplKind() const{
    return impl_kind_;
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

    bool bind_index = false;
    bool bind_labels = true;
    bool bind_types = true;

    //The only time we should bind the index is when we are contained in another instance
    if(instance_parent->isInstance()){
        bind_index = true;
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

    for(auto definition_key : bind_values.uniqueKeys()){
        for(auto instance_key : bind_values.values(definition_key)){
            LinkData(definition, definition_key, instance, instance_key, setup);
        }
    }
}

void Node::BindDataRelationship(Node* source, Node* destination, bool setup){
    if(source && destination && source->isNodeOfType(NODE_TYPE::DATA) && destination->isNodeOfType(NODE_TYPE::DATA)){
        auto source_parent = source->getParentNode();
        auto destination_parent = destination->getParentNode();

        //Bind the special vector linking
        if(destination_parent && destination_parent->getNodeKind() == NODE_KIND::WORKER_PROCESS){
            auto worker_name = destination_parent->getDataValue("worker").toString();
            auto parameter_label = destination->getDataValue("label").toString();

            //Check bindings
            if(worker_name == "Vector_Operations" && parameter_label.contains("Vector")){
                //Get the child type of the Vector

                //Get the siblings of the parameter
                for(auto param : destination_parent->getChildren(0)){
                    if(param->isNodeOfType(NODE_TYPE::PARAMETER)){
                        auto param_label = param->getDataValue("label").toString();
                        Node* bind_src = 0;

                        auto definition_key = "";
                        
                        if(param_label.contains("Value")){
                            definition_key = "inner_type";
                        }else if(param_label.contains("Vector")){
                            definition_key = "type";
                        }
                        LinkData(source, definition_key, param, "type", setup);
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