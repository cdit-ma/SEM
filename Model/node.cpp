#include "node.h"
#include <QDebug>
#include "graphmldata.h"
#include "edge.h"


Node::Node(Node::NODE_TYPE type) : GraphML(GraphML::NODE)
{
    //Set the Node Type.
    nodeType = type;

    //Initialize Instance Variables.
    definition = 0;
    parentNode = 0;
    sortOrder = -1;
    childCount = 0;
}

void Node::setSortPosition(int i)
{
    //Check
    sortOrder = i;
    GraphMLData* sortData = getData("sortOrder");
    if(sortData){
        sortData->setValue(QString::number(sortOrder));
    }
}

int Node::getSortPosition()
{
    return sortOrder;
}

QList<int> Node::getTreeIndex()
{
    return treeIndex;
}

Node::~Node()
{
    if(parentNode){
        parentNode->removeChild(this);
    }
}

void Node::setTop()
{
    this->treeIndex.append(0);
}

QString Node::toString()
{
    GraphMLData* kindData = getData("kind");
    GraphMLData* labelData = getData("label");
    QString kind = "Node";
    QString label = "Node";
    if(kindData){
        kind = kindData->getValue();
    }
    if(labelData){
        label = labelData->getValue();
    }
    return QString("[%1]%2 - %3").arg(getID(), kind, label);
}

Node *Node::getParentNode()
{
    return parentNode;
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

void Node::addChild(Node *child)
{
    if(child && !containsChild(child)){
        children << child;
        child->setParentNode(this, childCount++);
    }
}

QString Node::getNodeKind()
{
    return getDataValue("kind");
}

bool Node::containsChild(Node *child)
{
    return children.contains(child);
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

QList<Node *> Node::getChildren(int depth)
{
    QList<Node *> childList;

    childList += getOrderedChildNodes();
    //While we still have Children, Recurse
    if(depth != 0){
        //Add children's children.
        foreach(Node* child, getChildren(0)){
            childList += child->getChildren(depth - 1);
        }
    }

    return childList;
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

void Node::removeChildren()
{
    children.clear();\
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
    qCritical() << "TEST";
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

bool Node::canConnect(Node *node)
{
    if(isConnected(node)){
        return false;
    }
    if(node->getParentNode() == this || this->getParentNode() == node){
        return false;
    }
    return true;
}

Edge* Node::getConnectingEdge(Node *node)
{
    foreach(Edge* edge, edges){
        if(edge->contains(node)){
            return edge;
        }
    }
    return 0;
}

QList<Node *> Node::getAllConnectedNodes(QList<Node *> connectedNodes)
{
    foreach(Edge* edge, getEdges()){
        Node* nodeEnd = edge->getSource();
        if(nodeEnd == this){
            nodeEnd = edge->getDestination();
        }

        if(!connectedNodes.contains(nodeEnd)){
            connectedNodes.append(nodeEnd);
            foreach(Node* cNode, nodeEnd->getAllConnectedNodes(connectedNodes)){
                if(!connectedNodes.contains(cNode)){
                    connectedNodes.append(cNode);
                }
            }
        }
    }
    return connectedNodes;
}

bool Node::isConnected(Node *node)
{
    return getConnectingEdge(node) != 0;
}

bool Node::isIndirectlyConnected(Node *node)
{
    return (getAllConnectedNodes().contains(node) || node->getAllConnectedNodes().contains(this) || node == this);
}


bool Node::containsEdge(Edge *edge)
{
    return edges.contains(edge);
}



QList<GraphMLKey *> Node::getKeys(int depth)
{
    QList<GraphMLKey*> allKeys;
    QList<GraphMLData*> allData;

    allData += getData();
    foreach(GraphML* entity, getChildren(depth)){
        allData += entity->getData();
    }
    foreach(GraphML* entity, getEdges(depth)){
        allData += entity->getData();
    }

    foreach(GraphMLData* data, allData){
        GraphMLKey* key = data->getKey();
        if(!allKeys.contains(key)){
            allKeys += key;
        }
    }

    return allKeys;
}

void Node::removeEdges()
{
    edges.clear();
}

QString Node::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    tabSpace.fill('\t', indentationLevel);

    QString returnable = QString(tabSpace + "<node id =\"%1\">\n").arg(getID());

    foreach(GraphMLData* data, attachedData){
        returnable += data->toGraphML(indentationLevel + 1);
    }

    if(childrenCount() > 0){
        returnable += QString(tabSpace + "\t<graph id =\"g%1\">\n").arg(this->getID());
    }

    foreach(Node* child, getChildren(0)){
        returnable += child->toGraphML(indentationLevel + 2);
    }

    if(childrenCount() > 0){
        returnable += QString(tabSpace + "\t</graph>\n");
    }

    returnable += tabSpace + "</node>\n";
    return returnable;
}

bool Node::isDefinition()
{
    return nodeType == Node::NT_DEFINITION || nodeType == Node::NT_DEFINSTANCE;
}

bool Node::isInstance()
{
    return nodeType == Node::NT_INSTANCE || nodeType == Node::NT_DEFINSTANCE;
}

bool Node::isImpl()
{
    return nodeType == Node::NT_IMPL;
}

void Node::setDefinition(Node *def)
{
    if(isImpl() || isInstance()){
        definition = def;
    }
}

Node *Node::getDefinition()
{
    return definition;
}

void Node::unsetDefinition()
{
    definition = 0;
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
    }
}

void Node::removeEdge(Edge *edge)
{
    edges.removeAll(edge);
}

void Node::setParentNode(Node *parent, int index)
{
    this->treeIndex = parent->getTreeIndex();
    this->treeIndex.append(index);
    parentNode = parent;
}

QList<Node *> Node::getOrderedChildNodes()
{
    QMap<QString, Node*> orderedList;

    foreach(Node* child, children){
        QString sortID = child->getDataValue("sortOrder");
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
        }else{
            orderedList.insertMulti(4, edge);
        }
    }

    return orderedList.values();

}
