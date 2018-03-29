#include "behaviournode.h"
#include "condition.h"
#include "branch.h"
#include "parameter.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include <QDebug>
#include "../edge.h"



BehaviourNode::BehaviourNode(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){

    //Register DefaultData
    RegisterDefaultData(factory, kind, "comment", QVariant::String);
};

BehaviourNode::BehaviourNode(NODE_KIND kind) : Node(kind)
{
    setNodeType(NODE_TYPE::BEHAVIOUR);
    setAcceptsEdgeKind(EDGE_KIND::WORKFLOW);
    _isProducer = false;
    _isReciever = false;
    is_top_level_ = false;
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
        foreach(Edge* edge, getEdges(0, EDGE_KIND::WORKFLOW)){
            if(edge->getDestination() == this){
                BehaviourNode* source = (BehaviourNode*)edge->getSource();
                if(!node){
                    node = source;
                }
                if(source->isNodeOfType(NODE_TYPE::BRANCH)){
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
    foreach(Edge* edge, getEdges(0, EDGE_KIND::WORKFLOW)){
        if(edge->getSource() == this){
            nodes.append((BehaviourNode*)edge->getDestination());
        }
    }
    return nodes;
}


BehaviourNode *BehaviourNode::getParentBehaviourNode()
{
    Node* node = getParentNode();
    if(node && node->isNodeOfType(NODE_TYPE::BEHAVIOUR)){
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

void BehaviourNode::setTopLevel(bool top_level){
    is_top_level_ = top_level;
}

bool BehaviourNode::isTopLevel() const{
    return is_top_level_;
}

bool BehaviourNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

bool BehaviourNode::canAcceptEdge(EDGE_KIND edgeClass, Node *dst)
{
    if(!acceptsEdgeKind(edgeClass)){
        return false;
    }

    switch(edgeClass){
    case EDGE_KIND::WORKFLOW:{
        if(!dst->isNodeOfType(NODE_TYPE::BEHAVIOUR)){
            return false;
        }

        if(!dst->isInModel()){
            return false;
        }

        BehaviourNode* bNode = (BehaviourNode*) dst;

        if(bNode->getNodeKind() == NODE_KIND::TERMINATION){
            Branch* branch = ((Termination*)bNode)->getBranch();
            BehaviourNode* initialProducer = getInitialProducer();

            if(branch){
                if(initialProducer->isNodeOfType(NODE_TYPE::CONDITION)){
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
                if(isNodeOfType(NODE_TYPE::BRANCH)){
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
                //qCritical() << "GOT PRODUCER ALREADY";
                return false;
            }
        }

        if(isWorkflowProducer() && !getRecieverNodes().empty()){
            //Only one
            return false;
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

bool BehaviourNode::requiresEdgeKind(EDGE_KIND edgeKind)
{
    if(getAcceptedEdgeKinds().contains(edgeKind)){
        switch(edgeKind){
        case EDGE_KIND::WORKFLOW:{
            bool needOutput = isWorkflowProducer() && getRecieverNodes().isEmpty();
            bool needInput = isWorkflowReciever() && !getProducerNode();
            if(!needOutput && !needInput){
                //Don't require a Workflow Edge
                return false;
            }
        }
        default:
            break;
        }
    }
    return Node::requiresEdgeKind(edgeKind);
}
