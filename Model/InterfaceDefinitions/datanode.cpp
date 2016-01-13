#include "datanode.h"

DataNode::DataNode(Node::NODE_TYPE type):BehaviourNode(type)
{
    setIsNonWorkflow(true);
    setIsDataInput(true);
    setIsDataOutput(true);
    setAcceptEdgeClass(Edge::EC_DATA);
}

DataNode::~DataNode()
{

}

bool DataNode::canConnect_DataEdge(Node *node)
{
    int depthToAspectChild = getDepthToAspect() - 1;

    Node* aspectChild = getParentNode(depthToAspectChild);
    if(aspectChild){
        if(!aspectChild->isImpl()){
            //Cannot have data edges for anything outside of an impl.
            return false;
        }
    }

    int depthToContainer = depthToAspectChild - 1;
    int depthToNodeContainer = node->getDepthToAspect() - 2;

    Node* container = getParentNode(depthToContainer);
    Node* nodeContainer = node->getParentNode(depthToNodeContainer);
    if(container == nodeContainer){
        //Can't connect a member instance in an aggregate to an aggregate.
        return false;
    }


    bool okay =  BehaviourNode::canConnect_DataEdge(node);
    return okay;
}

