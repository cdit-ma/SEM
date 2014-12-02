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
    implementation = 0;
    parentNode = 0;

}

Node::~Node()
{
    if(parentNode){
        parentNode->removeChild(this);
    }

}

Node *Node::getParentNode()
{
    return parentNode;
}

bool Node::canAdoptChild(Node *node)
{
    Q_CHECK_PTR(node);

    //Check for Cyclic contains.
    if(node == this){
        qWarning() << "Cannot Adopt self.";
        return false;
    }

    if(containsChild(node) || isAncestorOf(node) || isDescendantOf(node)){
        qCritical() << "Cannot Adopt Ancestor/Descendant";
        return false;
    }

    return true;
}

void Node::addChild(Node *child)
{
    Q_CHECK_PTR(child);

    if(!containsChild(child)){
        children.append(child);
        foreach(QString aspect, getAspects()){
            child->addAspect(aspect);
        }

        child->setParentNode(this);
    }
}

bool Node::containsChild(Node *child)
{
    Q_CHECK_PTR(child);
    return children.contains(child);
}

QVector<Node *> Node::getChildren(int depth)
{
    QVector<Node *> childList;

    //Add direct Children
    childList += children;

    //While we still have Children, Recurse
    if(depth != 0){
        //Add children's children.
        foreach(Node* child, getChildren(0)){
            childList += child->getChildren(depth - 1);
        }
    }

    return childList;
}

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
    Q_CHECK_PTR(child);

    if(containsChild(child)){
        //Remove Child.
        int index = children.indexOf(child);
        children.removeAt(index);
    }
}

void Node::removeChildren()
{
    while(!children.isEmpty()){
        Node* current = children.first();
        children.removeFirst();
        delete current;
    }
}

bool Node::isAncestorOf(Node *node)
{
    return getChildren().contains(node);
}

bool Node::isDescendantOf(Node *node)
{
    if(parentNode){
        if(parentNode == node || parentNode->containsChild(node)){
            return true;
        }else{
            return parentNode->isDescendantOf(node);
        }
    }
    return false;
}

bool Node::canConnect(Node *node)
{
    if(isConnected(node)){
        //qWarning() << "Cannot connect 2 Connected Nodes";
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

bool Node::isConnected(Node *node)
{
    return getConnectingEdge(node) != 0;
}

bool Node::containsEdge(Edge *edge)
{
    return edges.contains(edge);
}

QVector<Edge *> Node::getEdges(int depth)
{
    QVector<Edge *> edgeList;

    foreach(Edge* edge, edges){
        if(!edgeList.contains(edge)){
            edgeList += edge;
        }
    }

    if(depth != 0){
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

QVector<GraphMLKey *> Node::getKeys(int depth)
{
    QVector<Node *> gChildren = getChildren(depth);
    QVector<Edge *> gEdges = getEdges(depth);

    QVector<GraphMLKey *> allKeys;


    foreach(GraphMLData* data, getData()){
        if(!allKeys.contains(data->getKey())){
            allKeys += data->getKey();
        }
    }

    foreach(Node* child, gChildren){
        foreach(GraphMLData* data, child->getData()){
            if(!allKeys.contains(data->getKey())){
                allKeys += data->getKey();
            }
        }
    }

    foreach(Edge* edge, gEdges){
        foreach(GraphMLData* data, edge->getData()){
            if(!allKeys.contains(data->getKey())){
                allKeys += data->getKey();
            }
        }
    }


    return allKeys;

}

void Node::removeEdges()
{
    while(!edges.isEmpty()){
        Edge* current = edges.first();
        edges.removeFirst();
        delete current;
    }
}

void Node::addAspect(QString aspect)
{
    if(!isInAspect(aspect)){
        containedAspects += aspect;
    }
}

void Node::removeAspect(QString aspect)
{
    if(isInAspect(aspect)){
        int index = containedAspects.indexOf(aspect);
        containedAspects.removeAt(index);
    }
}

bool Node::isInAspect(QString aspect)
{
    return containedAspects.contains(aspect);
}

QVector<QString> Node::getAspects()
{
    return containedAspects;
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
        //qCritical() << toString() << " Added " << inst->toString() << "  as Instance";
        if(!instances.contains(inst)){
            instances.append(inst);
            inst->setDefinition(this);
        }
    }
}

QVector<Node *> Node::getInstances()
{
    return instances;
}

void Node::removeInstance(Node *inst)
{
    if(isDefinition()){
        int index = instances.indexOf(inst);
        if(index != -1){
            inst->unsetDefinition();
            instances.remove(index);
        }
    }
}

void Node::addImplementation(Node *impl)
{
    if(isDefinition()){
        if(!implementations.contains(impl)){
            implementations.append(impl);
            impl->setDefinition(this);
        }
    }
}

QVector<Node *> Node::getImplementations()
{
    return implementations;
}

void Node::removeImplementation(Node *impl)
{
    if(isDefinition()){
        int index = implementations.indexOf(impl);
        if(index != -1){
            impl->unsetDefinition();
            implementations.remove(index);
        }
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
    if(containsEdge(edge)){
        int index = edges.indexOf(edge);
        edges.removeAt(index);
    }
}

void Node::setParentNode(Node *parent)
{
    parentNode = parent;

}
