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

    return BehaviourNode::canConnect_DataEdge(node);
}

