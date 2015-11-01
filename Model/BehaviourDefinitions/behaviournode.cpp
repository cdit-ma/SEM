#include "behaviournode.h"
#include "branch.h"
#include "termination.h"
#include "condition.h"
#include "parameter.h"

#include <QDebug>
//Static definition
QHash<QString, QList<ParameterRequirement*> > BehaviourNode::_parameters;


BehaviourNode::BehaviourNode(bool isStart, bool isEnd, bool restrictDepth, Node::NODE_TYPE type):Node(type)
{
    this->_isStart = isStart;
    this->_isEnd = isEnd;
    this->_restrictDepth = restrictDepth;
    _isUnconnectable = false;

}

bool BehaviourNode::isStart()
{
    return _isStart;
}

bool BehaviourNode::isEnd()
{
    return _isEnd;
}

bool BehaviourNode::isUnconnectable()
{
    return _isUnconnectable;
}

void BehaviourNode::addParameter(QString kind, QString name, QString type, bool inputParameter, QString defaultValue)
{
    ParameterRequirement* param = new ParameterRequirement(name, type, inputParameter, defaultValue);
    qCritical() << "Added Parameter: " << name << " To Kind: " << kind;
    _parameters[kind].append(param);
}

QList<ParameterRequirement *> BehaviourNode::getParameters()
{
    return _parameters[getNodeKind()];
}

bool BehaviourNode::hasParameters()
{
    return _parameters.contains(getNodeKind());
}

void BehaviourNode::setUnconnectable(bool unconnectable)
{
    this->_isUnconnectable = unconnectable;
}

bool BehaviourNode::canConnect(Node *attachableObject)
{

    if((isInstance() || isImpl()) && !getDefinition() && attachableObject->isDefinition()){
        return Node::canConnect(attachableObject);
    }
    //Limit connections to the parent (ie ComponentImpl) children

    Node* parentNode = getParentNode();
    while(parentNode && parentNode->getNodeKind() != "ComponentImpl"){
        parentNode = parentNode->getParentNode();
    }
    if(parentNode){
        if(!parentNode->isAncestorOf(attachableObject)){
            //qCritical() << "Reason # 1";
            return false;
        }
    }

    BehaviourNode* behaviourNode = dynamic_cast<BehaviourNode*>(attachableObject);
    if(behaviourNode){
        bool checkIndirectMatches = true;

        Termination* termination = dynamic_cast<Termination*>(attachableObject);
        if(termination){

            bool gotMatch = false;
            Node* leftNode = this;
            while(leftNode){
                Termination* t = dynamic_cast<Termination*>(leftNode);
                Branch* b = dynamic_cast<Branch*>(leftNode);
                Condition* c = dynamic_cast<Condition*>(leftNode);
                if(t){
                    //Jump to the Branch of the T
                    leftNode = t->getBranch();
                }else{
                    if(c){
                        if(c->getTermination()){
                            leftNode = c->getBranch();
                        }else{
                            if(c->getBranch()->getTermination() == termination){
                                gotMatch = true;
                                break;
                            }else{
                                //qCritical() << "Reason # 2";
                                return false;
                            }
                        }
                    }else{
                        if(b && b == this){
                            gotMatch = true;
                            break;
                        }
                        QList<Node*> leftNodes = leftNode->getNodesLeft();
                        if(leftNodes.length() == 0){
                            leftNode = 0;
                        }else{
                            //qCritical() << leftNodes.length();
                            foreach(Node* lNode, leftNode->getNodesLeft()){
                                if(lNode){
                                    leftNode = lNode;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if(!gotMatch){
                //qCritical() << "Reason # 3";
                return false;
            }else{
                checkIndirectMatches = false;
            }
        }

        if(behaviourNode->isStart()){
            return false;
        }

        //If this node is trying to connect, check if we have an out Connection.
        foreach(Edge* edge, this->getEdges(0)){
            if(!(edge->isImplLink() || edge->isInstanceLink()) && edge->getSource() == this){
                //qCritical() << "Reason # 4";
                return false;
            }
        }
        if(!termination){
            //If the node we are trying to connect to already has an In Connection
            foreach(Edge* edge, attachableObject->getEdges(0)){
                if(!(edge->isImplLink() || edge->isInstanceLink()) && edge->getDestination() == attachableObject){
                    //qCritical() << "Reason # 5";
                    return false;
                }
            }
        }



        if(checkIndirectMatches && isIndirectlyConnected(attachableObject)){
            if(!(this->isImpl() || attachableObject->isImpl())){
                //qCritical() << "Reason # 6";
                return false;
            }
        }
    }else{
        //qCritical() << "Reason # 7";
        return false;
    }
    return Node::canConnect(attachableObject);
}

bool BehaviourNode::canAdoptChild(Node *child)
{
    //Allow Parameters.
    Parameter* parameter = dynamic_cast<Parameter*>(child);
    if(parameter){
        if(!needsParameter(parameter)){
            return false;
        }
    }

    return Node::canAdoptChild(child);
}

bool BehaviourNode::needsParameter(Parameter *p)
{
    QList<ParameterRequirement*> neededParameters = _parameters[getNodeKind()];

    foreach(Node* child, getChildren(0)){
        Parameter* childParam = dynamic_cast<Parameter*>(child);
        if(childParam){

            ParameterRequirement* matchingParam = 0;
            foreach(ParameterRequirement* param, neededParameters){
                if(param->matches(childParam)){
                    matchingParam = param;
                    break;
                }
            }
            if(matchingParam){
                neededParameters.removeAll(matchingParam);
            }
        }
    }
    if(neededParameters.isEmpty()){
        return false;
    }

    foreach(ParameterRequirement* param, neededParameters){
         if(param->matches(p)){
            return true;
        }
    }
    return false;
}




ParameterRequirement::ParameterRequirement(QString name, QString type, bool inputParameter, QString defaultValue)
{
    this->name = name;
    this->type = type;
    this->inputParam = inputParameter;
    this->value = defaultValue;
}

QString ParameterRequirement::getName()
{
    return name;
}

QString ParameterRequirement::getType()
{
    return type;
}

QString ParameterRequirement::getValue()
{
    return value;
}

bool ParameterRequirement::isInputParameter()
{
    return inputParam;
}

bool ParameterRequirement::isReturnParameter()
{
    return !inputParam;
}

bool ParameterRequirement::matches(Parameter *p)
{
    if(p){
        if(p->getDataValue("label") != getName()){
            return false;
        }
        if(p->getDataValue("type") != getType()){
            return false;
        }
        return true;
    }
    return false;
}
