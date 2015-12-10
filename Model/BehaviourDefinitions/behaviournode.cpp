#include "behaviournode.h"
#include "condition.h"
#include "branch.h"

BehaviourNode::BehaviourNode(Node::NODE_TYPE type):Node(type)
{
    //Setup initial sets.
    _isWorkflowStart = false;
    _isWorkflowEnd = false;
    _isNonWorkflow = false;

    addValidEdgeType(Edge::EC_WORKFLOW);
}

void BehaviourNode::setIsWorkflowStart(bool start)
{
    _isWorkflowStart = start;
}

void BehaviourNode::setIsWorkflowEnd(bool end)
{
    _isWorkflowEnd = end;
}

void BehaviourNode::setIsNonWorkflow(bool nonWorkflow)
{
    _isNonWorkflow = nonWorkflow;
}

bool BehaviourNode::isWorkflowStart()
{
    return _isWorkflowStart;
}

bool BehaviourNode::isWorkflowEnd()
{
    return _isWorkflowEnd;
}

bool BehaviourNode::isNonWorkflow()
{
    return _isNonWorkflow;
}

bool BehaviourNode::gotLeftWorkflowEdge()
{
    if(isNonWorkflow() || isWorkflowStart()){
        return true;
    }
    return getLeftWorkflowEdge() != 0;
}

WorkflowEdge *BehaviourNode::getLeftWorkflowEdge()
{
    foreach(Edge* edge, getEdges(0)){
        if(edge->getEdgeClass() == Edge::EC_WORKFLOW){
            if(edge->getDestination() == this){
                return (WorkflowEdge*)edge;
            }
        }
    }
    return 0;
}

WorkflowEdge *BehaviourNode::getRightWorkflowEdge()
{
    foreach(Edge* edge, getEdges(0)){
        if(edge->getEdgeClass() == Edge::EC_WORKFLOW){
            if(edge->getSource() == this){
                return (WorkflowEdge*)edge;
            }
        }
    }
    return 0;

}

BehaviourNode *BehaviourNode::getRightBehaviourNode()
{
    WorkflowEdge* edge = getRightWorkflowEdge();
    if(edge){
        BehaviourNode* behaviourNode = dynamic_cast<BehaviourNode*>(edge->getDestination());
        if(behaviourNode){
            return behaviourNode;
        }
    }
    return 0;
}

BehaviourNode *BehaviourNode::getLeftBehaviourNode()
{
    WorkflowEdge* edge = getLeftWorkflowEdge();
    if(edge){
        BehaviourNode* behaviourNode = dynamic_cast<BehaviourNode*>(edge->getSource());
        if(behaviourNode){
            return behaviourNode;
        }
    }
    return 0;
}

BehaviourNode *BehaviourNode::getParentBehaviourNode()
{
    Node* node = getParentNode();
    BehaviourNode* parentBehaviour = dynamic_cast<BehaviourNode*>(node);
    return parentBehaviour;
}

bool BehaviourNode::isRelatedToBehaviourNode(BehaviourNode *relative)
{
    BehaviourNode* node = this;

    QList<BehaviourNode*> relatedNodes;
    while(node){
        BehaviourNode* leftNode = node->getLeftBehaviourNode();

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


BehaviourNode *BehaviourNode::getStartOfWorkflowChain()
{
    BehaviourNode* node = this;

    while(node){
        BehaviourNode* leftNode = node->getLeftBehaviourNode();

        if(!leftNode){
            return node;
        }
        node = leftNode;
    }
    return 0;
}

BehaviourNode *BehaviourNode::getEndOfWorkflowChain()
{
    BehaviourNode* node = this;

    while(node){
        BehaviourNode* rightNode = node->getRightBehaviourNode();

        if(!rightNode){
            return node;
        }
        node = rightNode;
    }
    return 0;
}

bool BehaviourNode::gotRightWorkflowEdge()
{
    if(isNonWorkflow() || isWorkflowEnd()){
        return true;
    }
    return getRightWorkflowEdge() != 0;
}

bool BehaviourNode::needEdge()
{
    return !(gotLeftWorkflowEdge() && gotRightWorkflowEdge());
}

bool BehaviourNode::canConnect_WorkflowEdge(Node *node)
{
    BehaviourNode* behaviourNode = dynamic_cast<BehaviourNode*>(node);

    if(!behaviourNode){
        //Can't connect a workflow edge to a non Behaviour Node
        return false;
    }

    if(gotRightWorkflowEdge()){
        //Cannot connect to an item if we already have a right edge.
        return false;
    }

    if(behaviourNode){
        //Check if the item we are connecting to is a Termination
        Termination* termination = dynamic_cast<Termination*>(node);

        if(termination){
            //Get the Branch connected to the Termination
            Branch* terminationsBranch = termination->getBranch();
            //Get the Start of this Workflow chain
            BehaviourNode* chainStart = getStartOfWorkflowChain();
            Condition* conditionChainStart = dynamic_cast<Condition*>(chainStart);

            if(terminationsBranch){
                //If we are connecting to a Termination which has a Branch.
                if(conditionChainStart){
                    if(chainStart->getParentNode() != terminationsBranch){
                        //If the Parent of the Condition at the start of this branch isn't The Branch connected to this termination.
                        return false;
                    }
                }
            }else{
                //Check if we connecting to an unconnected Termination.
                Branch* branch = dynamic_cast<Branch*>(this);
                if(!branch){
                    //Can only connect a Termination if the Termination doesn't yet have a Branch.
                    return false;
                }
            }
        }else{
            if(behaviourNode->gotLeftWorkflowEdge()){
                //Cannot connect to a Behaviour Node which has an Left Edge already.
                return false;
            }
        }

        if(isRelatedToBehaviourNode(behaviourNode)){
            //Stop Cycles
            return false;
        }
    }



    return Node::canConnect_WorkflowEdge(node);
}


bool BehaviourNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

