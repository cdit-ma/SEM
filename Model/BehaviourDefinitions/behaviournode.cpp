#include "behaviournode.h"
#include "branch.h"
#include "termination.h"
#include "condition.h"

#include <QDebug>
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
    return Node::canAdoptChild(child);
}

