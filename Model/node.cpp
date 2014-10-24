#include "node.h"
#include <QDebug>
#include "graphmldata.h"

int Node::_Nid = 0;

Node::Node(QString name, Node::NODE_TYPE nodeType): GraphMLContainer(GraphML::NODE, name)
{
    this->nodeType = nodeType;
    unsetDefinition();
    unsetImplementation();
}

Node::~Node(){
    removeEdges();
    removeChildren();

    if(this->getParent()){
        this->getParent()->disown(this);
    }
    emit destructGUI(this, getID());
}

void Node::addAspect(QString aspect)
{
    if(!inAspect(aspect)){
        containedAspects.append(aspect);
    }

}

void Node::removeAspect(QString aspect)
{
    if(inAspect(aspect)){
        containedAspects.removeAt(containedAspects.indexOf(aspect));
    }
}

bool Node::inAspect(QString aspect)
{
    return this->containedAspects.contains(aspect);
}


Node *Node::getParentNode()
{
    //Graph* parentGraph = dynamic_cast<Graph*>(getParent());
    Node* parentNode = dynamic_cast<Node*>(getParent());

    if(parentNode){
        return parentNode;
    }else{
        return 0;
    }
    /*
    Node* parentNode = 0;
    if(parentGraph != 0){
        parentNode = dynamic_cast<Node*>(parentGraph->getParent());
    }

    if(parentGraph != 0 && parentNode != 0){
        return parentNode;
    }
    return 0;
    */
}

QString Node::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    tabSpace.fill('\t', indentationLevel);

    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());

    for(int i=0; i < attachedData.size();i++){
        returnable += attachedData[i]->toGraphML(indentationLevel+1);
    }

    if(descendants.size() > 0){
        returnable += tabSpace + QString("\t<graph id =\"g%1\">\n").arg(this->getID());
    }
    for(int i=0; i < descendants.size(); i++){
        returnable += descendants[i]->toGraphML(indentationLevel+2);
    }

    if(descendants.size() > 0){
        returnable += tabSpace + QString("\t</graph>\n");
    }

    returnable += tabSpace + "</node>\n";
    return returnable;
}


QString Node::toString()
{
    return QString("Node[%1]: "+this->getName()).arg(this->getID());
}


bool Node::isDefinition()
{
    return nodeType == NT_DEFINITION;
}

bool Node::isInstance()
{
    return nodeType == NT_INSTANCE;
}


bool Node::isImpl()
{
    return nodeType == NT_IMPL;
}

void Node::setDefinition(Node *def)
{
    if(isImpl() || isInstance()){
        definition = def;
    }
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

void Node::removeInstance(Node *inst)
{
    if(isDefinition()){
        int index = instances.indexOf(inst);
        if(index != -1){
            inst->unsetDefinition();
            instances.removeAt(index);
        }
    }
}

void Node::setImplementation(Node *impl)
{
    if(isDefinition()){
        implementation = impl;
        impl->setDefinition(this);
    }
}

void Node::unsetImplementation()
{
    implementation = 0;
}

