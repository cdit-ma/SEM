#include "node.h"
#include "data.h"
#include "edge.h"
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>

Node::Node(NODE_TYPE type, NODE_CLASS nClass) : Entity(EK_NODE)
{
    //Set the Node Type.
    nodeType = type;

    //Initialize Instance Variables.
    definition = 0;
    parentNode = 0;
    childCount = 0;

    nodeClass = nClass;
}

QString Node::toGraphML(int indentDepth)
{
    return _toGraphML(indentDepth, false);
}

QString Node::toGraphMLNoVisualData(int indentDepth)
{
    return _toGraphML(indentDepth, true);
}


QList<int> Node::getTreeIndex()
{
    return treeIndex;
}

QString Node::getTreeIndexString()
{
    return treeIndexStr;
}

NODE_CLASS Node::getNodeClass()
{
    return nodeClass;
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
void Node::setAcceptEdgeClass(Edge::EDGE_CLASS validEdge)
{
    if(!validEdges.contains(validEdge)){
        validEdges.append(validEdge);
    }
}

bool Node::acceptsEdgeClass(Edge::EDGE_CLASS edgeClass)
{
    return validEdges.contains(edgeClass);
}

Node *Node::getContainedAspect()
{
    int depth = getDepthToAspect();
    return getParentNode(depth);
}

int Node::getDepthToAspect()
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


void Node::setTop(int index)
{
    this->treeIndex.append(index);
    this->treeIndexStr = QString::number(index);
}

QString Node::toString()
{
    Data* kindData = getData("kind");
    Data* labelData = getData("label");
    QString kind = "Node";
    QString label = "Node";
    if(kindData){
        kind = kindData->getValue().toString();
    }
    if(labelData){
        label = labelData->getValue().toString();
    }
    return QString("[%1]%2 - %3").arg(QString::number(getID()), kind, label);
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

void Node::addChild(Node *child)
{
    if(child && !containsChild(child)){
        children << child;
        child->setParentNode(this, childCount++);
    }
}

QString Node::getNodeKind()
{
    return getDataValue("kind").toString();
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



QList<Edge *> Node::getEdges(int depth)
{

    QList<Edge *> edgeList;

    edgeList += getOrderedEdges();
    //While we still have Children, Recurse
    if(depth != 0){
        //Add children's children.
        foreach(Node* child, getChildren(0)){
            foreach(Edge* edge, child->getEdges(depth - 1)){
                if(!edgeList.contains(edge)){
                    edgeList += edge;
                }
            }
        }
    }
    return edgeList;
}


Node *Node::getFirstChild()
{
    return getOrderedChildNodes().first();
}

Edge *Node::getFirstEdge()
{
    return getOrderedEdges().first();
}

QList<Node *> Node::getSiblings()
{
    QList<Node *> childList;
    if(getParentNode()){
        foreach(Node* sibling, getParentNode()->getChildren(0)){
            if(sibling!= this){
                childList << sibling;
            }
        }
    }
    return childList;
}

QList<Node *> Node::getChildrenOfKind(QString kindStr, int depth)
{
    QList<Node *> childList = getChildren(depth);

    QList<Node*> returnableList;
    foreach(Node* childNode, getChildren(depth)){
        if(childNode->getDataValue("kind") == kindStr){
            returnableList.append(childNode);
        }
    }
    return returnableList;
}

QList<int> Node::getEdgeIDs(Edge::EDGE_CLASS edgeClass)
{
    QList<int> returnable;
    foreach(Edge* edge, getEdges(0)){
        if(edgeClass == Edge::EC_NONE || edge->getEdgeClass() == edgeClass){
            returnable += edge->getID();
        }
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

int Node::edgeCount()
{
    return edges.size();
}

void Node::removeChild(Node *child)
{
    if(child){
        children.removeAll(child);
    }
}


bool Node::ancestorOf(Node *node)
{
    QList<int> otherTree = node->getTreeIndex();

    if(this == node){
        return true;
    }

    if(this->treeIndex.size() > otherTree.size()){
        return false;
    }

    for(int i=0; i<treeIndex.size(); i++){
        if(treeIndex.at(i) != otherTree.at(i)){
            return false;
        }
    }
    return true;
}

bool Node::ancestorOf(Edge *edge)
{
    return getEdges().contains(edge);
}

bool Node::isAncestorOf(GraphML *item)
{
    Node* node = dynamic_cast<Node*>(item);
    Edge* edge = dynamic_cast<Edge*>(item);
    if(edge){
        return ancestorOf(edge);
    }
    if(node){
        return ancestorOf(node);
    }
    return false;
}

bool Node::isDescendantOf(Node *node)
{
    QList<int> otherTree = node->getTreeIndex();

    if(this == node){
        return false;
    }
    if(this->treeIndex.size() > otherTree.size()){
        return false;
    }

    for(int i=0; i< otherTree.size(); i++){
        if(treeIndex.at(i) != otherTree.at(i)){
            return false;
        }
    }
    return true;
}

Edge::EDGE_CLASS Node::canConnect(Node *node)
{
    //Don't allow multiple connections.
    if(gotEdgeTo(node)){
        return Edge::EC_NONE;
    }

    //Don't allow connections to parents.
    if(node->getParentNode() == this || getParentNode() == node){
        return Edge::EC_NONE;
    }

    //Don't allow connections to self.
    if(node == this){
        return Edge::EC_NONE;
    }


    //Check if node can connect as a definition.
    if(acceptsEdgeClass(Edge::EC_DEFINITION)){
        //qCritical() << "Trying canConnect_DefinitionEdge";
        if(canConnect_DefinitionEdge(node)){
            return Edge::EC_DEFINITION;
        }
    }

    //Check if node can connect as an Aggregate.
    if(acceptsEdgeClass(Edge::EC_AGGREGATE)){
        //qCritical() << "Trying canConnect_AggregateEdge";
        if(canConnect_AggregateEdge(node)){
            return Edge::EC_AGGREGATE;
        }
    }

    //Check if node can connect as an Assembly.
    if(acceptsEdgeClass(Edge::EC_ASSEMBLY)){
        //qCritical() << "Trying canConnect_AssemblyEdge";
        if(canConnect_AssemblyEdge(node)){
            return Edge::EC_ASSEMBLY;
        }
    }

    //Check if node can connect as a Data.
    if(acceptsEdgeClass(Edge::EC_DATA)){
        //qCritical() << "Trying canConnect_DataEdge";
        if(canConnect_DataEdge(node)){
            return Edge::EC_DATA;
        }
    }

    //Check if node can connect as a Data.
    if(acceptsEdgeClass(Edge::EC_DEPLOYMENT)){
        //qCritical() << "Trying canConnect_DeploymentEdge";
        if(canConnect_DeploymentEdge(node)){
            return Edge::EC_DEPLOYMENT;
        }
    }

    //Check if node can connect as a Data.
    if(acceptsEdgeClass(Edge::EC_WORKFLOW)){
        //qCritical() << "Trying canConnect_WorkflowEdge";
        if(canConnect_WorkflowEdge(node)){
            return Edge::EC_WORKFLOW;
        }
    }

    return Edge::EC_NONE;
}

bool Node::canConnect_AggregateEdge(Node *)
{
    return true;
}

bool Node::canConnect_AssemblyEdge(Node *)
{
    return true;
}

bool Node::canConnect_DataEdge(Node *node)
{
    //Check for contained in same Aspect child.
    Node* componentImpl = getParentNode(getDepthToAspect() - 1);
    Node* nodeComponentImpl = node->getParentNode(node->getDepthToAspect() - 1);

    if(componentImpl != nodeComponentImpl){
        return false;
    }
    if(!componentImpl){
        return false;
    }
    if(!nodeComponentImpl){
        return false;
    }

    return true;
}

/**
 * @brief Node::canConnect_DefinitionEdge Returns whether or not this Node can be made an Instance/Impl of the definition node provided
 * @param definition - The node which will be a definition.
 * @return true/false
 */
bool Node::canConnect_DefinitionEdge(Node *definition)
{
    //This must be an Instance/Impl Node Type
    if(!(isInstance() || isImpl())){
        return false;
    }

    //Node must be a Definition Node Type.
    if(!definition->isDefinition()){
        return false;
    }

    //Node cannot already have a Definition.
    if(getDefinition()){
        return false;
    }


    //Check parentNode
    Node* parentNode = getParentNode();

    if(parentNode && (parentNode->isInstance() || parentNode->isImpl())){
        Node* parentDefinition = parentNode->getDefinition();
        if(parentDefinition){
            if(!parentDefinition->isAncestorOf(definition)){
                //An Entity cannot be connected to It's definition if it's not contained in the parents definition Entity.
                return false;
            }
        }
    }

    //Check for cyclic stuff
    foreach(Node* child, definition->getChildren()){
        if(child->isInstance() && child->getDefinition()){
            Node* def = child->getDefinition(true);

            if(def){
                if(def->isAncestorOf(this)){
                    //Disallow cycles.
                    return false;
                }
            }
        }
    }

    return true;
}

bool Node::canConnect_DeploymentEdge(Node *hardware)
{

    if(!hardware->isHardware()){
        //If the node we are trying to connect to isn't a HardwareType, then return false.
        return false;
    }

    foreach(Edge* edge, getEdges(0)){
        if(edge->getEdgeClass() == Edge::EC_DEPLOYMENT){
            //There can only be one Deployment edge.
            return false;
        }
    }

    return true;
}

bool Node::canConnect_WorkflowEdge(Node *node)
{
    //Check for contained in same Aspect child.
    Node* componentImpl = getParentNode(getDepthToAspect() - 1);
    Node* nodeComponentImpl = node->getParentNode(node->getDepthToAspect() - 1);

    if(componentImpl != nodeComponentImpl){
        return false;
    }
    if(!componentImpl){
        return false;
    }
    if(!nodeComponentImpl){
        return false;
    }

    return true;
}



Edge* Node::getEdgeTo(Node *node)
{
    foreach(Edge* edge, edges){
        if(edge->contains(node)){
            return edge;
        }
    }
    return 0;
}


bool Node::gotEdgeTo(Node *node)
{
    return getEdgeTo(node) != 0;
}

Edge *Node::getEdge(int ID)
{
    for(int i =0; i < edges.length(); i++){
        Edge* edge = edges[i];
        if(edge && edge->getID() == ID){
            return edge;
        }
    }
    return 0;
}

QList<Node *> Node::getAllConnectedNodes()
{

    QList<Node*> currentNodes = getChildren();


    foreach(Edge* edge, getEdges()){

        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        if(!currentNodes.contains(src)){
            currentNodes << src;
        }
        if(!currentNodes.contains(dst)){
            currentNodes << dst;
        }


        if(edge->isInstanceLink()){
            Node* definition = dst->getDefinition();
            while(definition){
                if(!currentNodes.contains(definition)){
                    currentNodes << definition;
                }

                definition = definition->getDefinition();
            }
        }
    }

    return currentNodes;
}

bool Node::isIndirectlyConnected(Node *node)
{
    return (getAllConnectedNodes().contains(node) || node->getAllConnectedNodes().contains(this) || node == this);
}

bool Node::containsEdgeEndPoints(Edge *edge)
{
    Node* src = edge->getSource();
    Node* dst = edge->getDestination();

    return isAncestorOf(src) && isAncestorOf(dst);
}



bool Node::containsEdge(Edge *edge)
{
    return edges.contains(edge);
}



QList<Key *> Node::getKeys(int depth)
{
    QList<Key*> allKeys;
    QList<Data*> allData;

    allData += getData();
    foreach(Entity* entity, getChildren(depth)){
        allData += entity->getData();
    }
    foreach(Entity* entity, getEdges(depth)){
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

QString Node::getEntityName()
{
    return getNodeKind();
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

bool Node::isDefinition()
{
    return nodeType == Node::NT_DEFINITION || nodeType == Node::NT_DEFINSTANCE;
}

bool Node::isInstance()
{
    return nodeType == Node::NT_INSTANCE || nodeType == Node::NT_DEFINSTANCE;
}

bool Node::isAspect()
{
    return nodeType == Node::NT_ASPECT;
}

bool Node::isImpl()
{
    return nodeType == Node::NT_IMPL;
}

bool Node::isHardware()
{
    return nodeType == Node::NT_HARDWARE;
}

void Node::setDefinition(Node *def)
{
    if(isImpl() || isInstance()){
        definition = def;
        emit node_GotDefinition(true);
    }

}

Node *Node::getDefinition(bool recurse)
{
    Node* def = definition;

    if(recurse && def && def->getDefinition(false)){
        def = def->getDefinition(true);
    }
    return def;
}

void Node::unsetDefinition()
{
    definition = 0;
    emit node_GotDefinition(false);
}

void Node::addInstance(Node *inst)
{
    if(isDefinition()){
        if(!instances.contains(inst)){
            instances.append(inst);
            inst->setDefinition(this);
        }
    }
}

QList<Node *> Node::getInstances()
{
    return instances;
}

void Node::removeInstance(Node *inst)
{
    if(isDefinition()){
        instances.removeAll(inst);
        instances.removeAll(inst);
        inst->unsetDefinition();
    }
}

void Node::addImplementation(Node *impl)
{
    if(isDefinition()){
        if(!implementations.contains(impl)){
            implementations.push_back(impl);
            impl->setDefinition(this);
        }
    }
}

QList<Node *> Node::getImplementations()
{
    return implementations;
}

QList<Node *> Node::getDependants()
{
    QList<Node*> nodes;
    nodes += implementations;
    nodes += instances;
    return nodes;
}

void Node::removeImplementation(Node *impl)
{
    if(isDefinition()){
        implementations.removeAll(impl);
        impl->unsetDefinition();
    }

}


void Node::addEdge(Edge *edge)
{
    if(!containsEdge(edge)){
        edges.append(edge);
        emit node_EdgeAdded(edge->getID(), edge->getEdgeClass());
    }
}

void Node::removeEdge(Edge *edge)
{
    edges.removeAll(edge);
    if(edge){
        emit node_EdgeRemoved(edge->getID(), edge->getEdgeClass());
    }
}

void Node::setParentNode(Node *parent, int index)
{
    this->treeIndex = parent->getTreeIndex();
    this->treeIndexStr = parent->getTreeIndexString() + ",";
    this->treeIndexStr += QString::number(index);
    this->treeIndex.append(index);
    parentNode = parent;
}

QString Node::_toGraphML(int indentDepth, bool ignoreVisuals)
{
    QString tab("\t");
    //Construct the desired tab width.
    QString indent = tab.repeated(indentDepth);

    QString xml;
    xml += indent;
    xml += indent + QString("<node id =\"%1\">\n").arg(getID());


    foreach(Data* data, getData()){
        if(data->isVisualData() && ignoreVisuals){
            if(data->getKeyName() != "readOnly"){
                //Skip Visual data.
                continue;
            }
        }
        xml += data->toGraphML(indentDepth + 1);
    }

    if(childrenCount() > 0){
        xml += indent;
        xml += QString("\t<graph id =\"g%1\">\n").arg(getID());
    }

    foreach(Node* child, getChildren(0)){
        xml += child->toGraphML(indentDepth + 2);
    }

    if(childrenCount() > 0){
        xml += indent;
        xml += QString("\t</graph>\n");
    }
    xml += indent;
    xml += "</node>\n";

    return xml;
}

QList<Node *> Node::getOrderedChildNodes()
{
    QMap<int, Node*> orderedList;

    foreach(Node* child, children){
        int sortID = child->getDataValue("sortOrder").toInt();
        orderedList.insertMulti(sortID, child);
    }
    return orderedList.values();
}

QList<Edge *> Node::getOrderedEdges()
{
    QMap<int, Edge*> orderedList;

    foreach(Edge* edge, edges){
        if(edge->isAggregateLink()){
            orderedList.insertMulti(2,edge);
        }else if(edge->isImplLink()){
            orderedList.insertMulti(1,edge);
        }else if(edge->isInstanceLink()){
            orderedList.insertMulti(0,edge);
        }else if(edge->isDelegateLink()){
            orderedList.insertMulti(3,edge);
        }else if(edge->isTerminationLink()){
            orderedList.insertMulti(4,edge);
        }else{
            orderedList.insertMulti(5, edge);
        }
    }

    return orderedList.values();

}
