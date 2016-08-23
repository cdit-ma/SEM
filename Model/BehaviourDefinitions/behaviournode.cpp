#include "behaviournode.h"
#include "condition.h"
#include "branch.h"
#include "parameter.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include <QDebug>


BehaviourNode::BehaviourNode(Node::NODE_KIND kind):Node(kind)
{
    setNodeType(NT_BEHAVIOUR);
    setAcceptsEdgeKind(Edge::EC_WORKFLOW);
    _isProducer = false;
    _isReciever = false;
}

void BehaviourNode::setWorkflowProducer(bool producer)
{
    _isProducer = producer;
}

void BehaviourNode::setWorkflowReciever(bool reciever)
{
    _isReciever = reciever;
}

bool BehaviourNode::isWorkflowProducer() const
{
    return _isProducer;
}

bool BehaviourNode::isWorkflowReciever() const
{
    return _isReciever;
}

BehaviourNode *BehaviourNode::getProducerNode()
{
    if(isWorkflowReciever()){
        //Find Reciving edge;
        BehaviourNode* node = 0;
        foreach(Edge* edge, getEdges(0, Edge::EC_WORKFLOW)){
            if(edge->getDestination() == this){
                BehaviourNode* source = (BehaviourNode*)edge->getSource();
                if(!node){
                    node = source;
                }
                if(source->isNodeOfType(NT_BRANCH)){
                    return source;
                }
            }
        }
        return node;
    }
    return 0;
}

QList<BehaviourNode *> BehaviourNode::getRecieverNodes()
{
    QList<BehaviourNode*> nodes;

    //Find Recieving edge;
    foreach(Edge* edge, getEdges(0, Edge::EC_WORKFLOW)){
        if(edge->getSource() == this){
            nodes.append((BehaviourNode*)edge->getDestination());
        }
    }
    return nodes;
}


BehaviourNode *BehaviourNode::getParentBehaviourNode()
{
    Node* node = getParentNode();
    if(node && node->isNodeOfType(NT_BEHAVIOUR)){
        return (BehaviourNode*) node;
    }
    return 0;
}

bool BehaviourNode::isNodeInBehaviourChain(BehaviourNode *relative)
{
    BehaviourNode* node = this;

    QList<BehaviourNode*> relatedNodes;
    while(node){
        BehaviourNode* leftNode = node->getProducerNode();

        if(!leftNode){
            leftNode = node->getParentBehaviourNode();
            if(!leftNode){
                break;
            }
        }

        relatedNodes << leftNode;
        node = leftNode;
    }
    if(relatedNodes.contains(relative)){
        return true;
    }
    return false;
}

BehaviourNode *BehaviourNode::getInitialProducer()
{
    BehaviourNode* n = this;

    while(n){
        BehaviourNode* pNode = n->getProducerNode();

        if(pNode){
            n = pNode;
        }else{
            break;
        }
    }
    return n;
}



bool BehaviourNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

bool BehaviourNode::canAcceptEdge(Edge::EDGE_CLASS edgeClass, Node *dst)
{
    if(!acceptsEdgeKind(edgeClass)){
        return false;
    }

    switch(edgeClass){
    case Edge::EC_WORKFLOW:{
        if(!dst->isNodeOfType(NT_BEHAVIOUR)){
            return false;
        }

        if(!dst->isInModel()){
            return false;
        }

        BehaviourNode* bNode = (BehaviourNode*) dst;

        if(bNode->getNodeKind() == NK_TERMINATION){
            Branch* branch = ((Termination*)bNode)->getBranch();
            BehaviourNode* initialProducer = getInitialProducer();

            if(branch){
                if(initialProducer->getNodeKind() == NK_CONDITION){
                    Condition* condition = (Condition*) initialProducer;
                    if(condition->gotTermination()){
                        return false;
                    }else if(condition->getRequiredTermination() != dst){
                        //If the Termination of the condition's parent branch isn't the Termination we are trying to connect to, return false.
                        return false;
                    }
                }else{
                    //The thing we are connecting to isn't contained within a branch. So we should never need a Termiantion.
                    return false;
                }
            }else{
                if(isNodeOfType(NT_BRANCH)){
                    Branch* branch = (Branch*) this;
                    if(branch->getTermination()){
                        qCritical() << "NO TERMINATION?";
                        //Connecting to a non Branch or a Branchw with a Termination isn't allowed.
                        return false;
                    }
                }
            }


        }else{
            //Already got a connection in.
            if(bNode->getProducerNode()){
                qCritical() << "GOT PRODUCER ALREADY";
                return false;
            }
        }

        //Not a reciever!
        if(!bNode->isWorkflowReciever()){
            return false;
        }


        if(isNodeInBehaviourChain(bNode)){
            //Disallow cycles!
            return false;
        }
        break;
        }
    default:
        break;
    }

    return Node::canAcceptEdge(edgeClass, dst);
}
