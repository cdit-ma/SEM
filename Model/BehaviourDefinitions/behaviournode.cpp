#include "behaviournode.h"
#include "condition.h"
#include "branch.h"
#include "parameter.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include <QDebug>
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
    return !(gotLeftWorkflowEdge() && gotRightWorkflowEdge() && gotDataInput() && gotDataOutput());
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
        QString type1 = getDataValue("type").toString();
        QString type2 = node->getDataValue("type").toString();

        if(type1 == type2 && type1 != ""){
            //Allow direct matches.
            return true;
        }
        if(numberTypes.contains(type1) && numberTypes.contains(type2)){
            //Allow matches of numbers
            return true;
        }
        if(stringTypes.contains(type1) && stringTypes.contains(type2)){
            //Allow matches of Strings
            return true;
        }
        if(type2 == "WE_UTE_Vector"){
            //Allow Vector Connections to WE_UTE_Vector types
            VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(this);
            if(vectorInstance){
                return true;
            }
        }
        if(type1 == "WE_UTE_Vector"){
            //Allow Vector Connections from WE_UTE_Vector types to Vectors.
            VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(node);
            if(vectorInstance){
                return true;
            }
        }

        if(type1 == "WE_UTE_VariableArguments" || type2 == "WE_UTE_VariableArguments"){
            //Allow anything to connect into the Variable Argument Type.
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
    if(!node->isInModel()){
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
                    //If we are in a chain connected to a Condition
                    Branch* conditionBranch = conditionChainStart->getBranch();
                    if(conditionBranch && conditionBranch->getTermination() != node){
                        //If the Termiantion of the condition's parent branch isn't the Termination we are trying to connect to, return false.
                        return false;
                    }
                }else{
                    //The thing we are connecting to isn't contained within a branch. So we should never need a Termiantion.
                    return false;
                }
            }else{
                //Check if we connecting to an unconnected Termination.
                Branch* branch = dynamic_cast<Branch*>(this);
                if(!branch || (branch && branch->getTermination())){
                    //Connecting to a non Branch or a Branchw with a Termination isn't allowed.
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

        if(behaviourNode->getInputData()){
            return false;
        }
    }


    if(!compareableTypes(node)){
        //Different Types
        return false;
    }

    if(!Node::canConnect_DataEdge(node)){
        return false;
    }

    //Look up for Vectors.
    Node* parentNode = getParentNode();
    while(parentNode){
        QString nodeKind = parentNode->getNodeKind();

        if(nodeKind == "VectorInstance"){
            //Can't allow connections from children of VectorInstance
            return false;
        }else if(nodeKind == "OutEventPortImpl"){
            //Can't allow connections from children of OutEventPortImpls
            return false;
        }else if(parentNode->isAspect()){
            //If we get to an Aspect, return.
            break;
        }
        parentNode = parentNode->getParentNode();
    }

    Node* nodeParent = node->getParentNode();
    while(nodeParent){
        QString nodeKind = nodeParent->getNodeKind();

        if(nodeKind == "VectorInstance"){
            //Can't allow connections from children of VectorInstance
            return false;
        }else if(nodeKind == "InEventPortImpl"){
            //Can't allow connections into children of InEventPortImpl
            return false;
        }else if(nodeParent->isAspect()){
            break;
        }
        nodeParent = nodeParent->getParentNode();
    }

    return true;
}


bool BehaviourNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

