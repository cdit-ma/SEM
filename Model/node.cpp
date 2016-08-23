#include "node.h"
#include "data.h"
#include "edge.h"
#include <QDebug>
#include <QCryptographicHash>
#include <QStringBuilder>
#include <QByteArray>

Node::Node(Node::NODE_KIND kind):Entity(EK_NODE)
{
    this->nodeKind = kind;
    parentNode = 0;
    childCount = 0;
    definition = 0;
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

QString Node::getTreeIndexAlpha()
{
    return treeIndexStr2;
}

Node::NODE_KIND Node::getNodeKind() const
{
    return nodeKind;
}

NODE_CLASS Node::getNodeClass()
{
    return nodeClass;
}

Node::NODE_TYPE Node::getNodeType()
{
    return nodeType;
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
bool Node::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        qCritical() << this << " Doesn't accept Edge" << edgeKind;
        return false;
    }

    Node* parentNode = getParentNode();
    switch(edgeKind){
        case Edge::EC_DEFINITION:{
        //This must be an Instance/Impl Node Type
        if(!(isInstanceImpl())){
            //qCritical() << "ISN'T AN INSTANCE/IMPL";
            return false;
        }

        //Node cannot already have a Definition.
        if(getDefinition()){
            //qCritical() << "GOT DEFINITION";

            return false;
        }

        //Node must be a Definition Node Type.
        if(!dst->isDefinition()){
            //qCritical() << "DST ISNT DEFINITION";
            return false;
        }

        if(parentNode && parentNode->isInstanceImpl()){
            Node* pDef = parentNode->getDefinition();
            if(pDef && !pDef->isAncestorOf(dst)){
                //qCritical() << "NO BEUNO";
                //An Entity cannot be connected to It's definition if it's not contained in the parents definition Entity.
                return false;
            }
        }

        //HMM

        break;
    }
    case Edge::EC_AGGREGATE:{


    }
    default:
        break;
    }


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

bool Node::isNodeOfType(Node::NODE_TYPE type) const
{
    return types.contains(type);
}

bool Node::acceptsEdgeKind(Edge::EDGE_CLASS edgeKind) const
{
    return validEdgeKinds.contains(edgeKind);
}

void Node::setNodeType(Node::NODE_TYPE type)
{
    if(!types.contains(type)){
        switch(type){
            case Node::NT_DEFINITION:
            case Node::NT_IMPL:
            case Node::NT_INSTANCE:
                setAcceptsEdgeKind(Edge::EC_DEFINITION);
                break;
            case Node::NT_DATA:
                setAcceptsEdgeKind(Edge::EC_DATA);
                break;
        default:
            break;
        }

        types.append(type);
    }
}

void Node::setAcceptsEdgeKind(Edge::EDGE_CLASS edgeKind)
{
    if(!validEdgeKinds.contains(edgeKind)){
        validEdgeKinds.append(edgeKind);
    }
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

Node *Node::getCommonAncestor(Node *dst)
{
    int height = getHeightToCommonAncestor(dst);

    if(height != -1){
        return getParentNode(height);
    }
    return 0;
}

int Node::getHeightToCommonAncestor(Node *dst)
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
    this->treeIndex.append(index);
    this->treeIndexStr = QString::number(index);
    this->treeIndexStr2 = QChar('A' + index);
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

VIEW_ASPECT Node::getViewAspect()
{
    return aspect;
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

void Node::addChild(Node *child)
{
    if(child && !containsChild(child)){
        children << child;
        child->setParentNode(this, childCount++);
    }
}

QString Node::getNodeKindStr()
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



QList<Edge *> Node::getEdges(int depth, Edge::EDGE_CLASS edgeKind)
{

    QList<Edge *> edgeList;

    edgeList += getOrderedEdges(edgeKind);
    //While we still have Children, Recurse
    if(depth != 0){
        //Add children's children.
        foreach(Node* child, getChildren(0)){
            foreach(Edge* edge, child->getEdges(depth - 1, edgeKind)){
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



Edge* Node::getEdgeTo(Node *node, Edge::EDGE_CLASS edgeKind)
{
    foreach(Edge* edge, getEdges(0, edgeKind)){
        if(edge->contains(node)){
            return edge;
        }
    }
    return 0;
}


bool Node::gotEdgeTo(Node *node, Edge::EDGE_CLASS edgeKind)
{
    return getEdgeTo(node, edgeKind) != 0;
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
    return getNodeKindStr();
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
    return isNodeOfType(NT_DEFINITION);
}

bool Node::isInstance()
{
    return isNodeOfType(NT_INSTANCE);
}

bool Node::isInstanceImpl()
{
    return isInstance() || isImpl();
}

bool Node::isAspect()
{
    return isNodeOfType(NT_ASPECT);
}

bool Node::isImpl()
{
    return isNodeOfType(NT_IMPL);
}

bool Node::canAcceptEdgeClass(Edge::EDGE_CLASS edgeClass)
{
    return validEdges2.contains(edgeClass);
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
    this->treeIndexStr2 = parent->getTreeIndexAlpha() % QChar('A' + index);
    this->treeIndex.append(index);
    parentNode = parent;
    if(parent){
        //Set the view Aspect.
        setViewAspect(parent->getViewAspect());
    }else{
        setViewAspect(VA_NONE);
    }
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

QList<Edge *> Node::getOrderedEdges(Edge::EDGE_CLASS edgeKind)
{
    QMultiMap<Edge::EDGE_CLASS, Edge*> orderedList;

    foreach(Edge* edge, edges){
        if(edgeKind == Edge::EK_NONE || edge->getEdgeClass() == edgeKind){
            orderedList.insertMulti(edge->getEdgeClass(), edge);
        }
    }
    return orderedList.values();
}
