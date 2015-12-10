#include "behaviournode.h"
#include "condition.h"
#include "branch.h"
#include "parameter.h"

BehaviourNode::BehaviourNode(Node::NODE_TYPE type):Node(type)
{
    //Setup initial sets.
    _isWorkflowStart = false;
    _isWorkflowEnd = false;
    _isNonWorkflow = false;
    _isDataInput = false;
    _isDataOutput = false;

    setAcceptEdgeClass(Edge::EC_WORKFLOW);
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

void BehaviourNode::setIsDataInput(bool input)
{
    _isDataInput = input;
}

void BehaviourNode::setIsDataOutput(bool output)
{
    _isDataOutput = output;
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

bool BehaviourNode::isDataInput()
{
    return _isDataInput;
}

bool BehaviourNode::isDataOutput()
{
    return _isDataOutput;
}

bool BehaviourNode::gotDataInput()
{
    if(!isDataInput()){
        return true;
    }
    return getInputDataEdge() != 0;
}

bool BehaviourNode::gotDataOutput()
{
    if(!isDataOutput()){
        return true;
    }
    return getOutputDataEdge() != 0;
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

DataEdge *BehaviourNode::getInputDataEdge()
{
    foreach(Edge* edge, getEdges(0)){
        if(edge->getEdgeClass() == Edge::EC_DATA){
            if(edge->getDestination() == this){
                return (DataEdge*)edge;
            }
        }
    }
    return 0;
}

DataEdge *BehaviourNode::getOutputDataEdge()
{
    foreach(Edge* edge, getEdges(0)){
        if(edge->getEdgeClass() == Edge::EC_DATA){
            if(edge->getSource() == this){
                return (DataEdge*)edge;
            }
        }
    }
    return 0;
}

Node *BehaviourNode::getInputData()
{
    DataEdge* edge = getInputDataEdge();
    if(edge){
        return edge->getSource();
    }
    return 0;
}

Node *BehaviourNode::getOutputData()
{
    DataEdge* edge = getOutputDataEdge();
    if(edge){
        return edge->getDestination();
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

bool BehaviourNode::compareableTypes(Node *node)
{
    QStringList numberTypes;
    numberTypes << "ShortInteger" << "LongInteger" << "LongLongInteger";
    numberTypes << "UnsignedShortInteger" << "UnsignedLongInteger" << "UnsignedLongLongInteger";
    numberTypes << "FloatNumber" << "DoubleNumber" << "LongDoubleNumber";
    numberTypes << "Boolean" << "Byte";

    QStringList stringTypes;
    stringTypes << "String" << "WideString";

    if(node){
        //Types
        QString type1 = getDataValue("type");
        QString type2 = node->getDataValue("type");

        if(numberTypes.contains(type1) && numberTypes.contains(type2)){
            return true;
        }
        if(stringTypes.contains(type1) && stringTypes.contains(type2)){
            return true;
        }
    }
    return false;

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

bool BehaviourNode::canConnect_DataEdge(Node *node)
{

    BehaviourNode* behaviourNode = dynamic_cast<BehaviourNode*>(node);

    if(!behaviourNode){
        //Can't connect a Data edge to a non Behaviour Node
        return false;
    }

    if(!isDataOutput()){
        //Cannot connect Data if we aren't an output.
        return false;
    }

    if(behaviourNode){
        if(!behaviourNode->isDataInput()){
            return false;
        }
        Parameter* parameter = dynamic_cast<Parameter*>(node);


        if(parameter && parameter->getInputData()){
            return false;
        }
    }

    if(!compareableTypes(node)){
        //Different Types
        return false;
    }

    //if(node->acceptsEdgeClass(Edge::EC_ASSEMBLY)){

    //}



    return Node::canConnect_DataEdge(node);
}


bool BehaviourNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

