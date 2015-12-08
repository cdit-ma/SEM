#include "behaviournode.h"

BehaviourNode::BehaviourNode(bool isStart, bool isEnd, bool restrictDepth, Node::NODE_TYPE type):Node(type)
{
    this->_isStart = isStart;
    this->_isEnd = isEnd;
    this->_restrictDepth = restrictDepth;
    _isUnconnectable = false;

    addValidEdgeType(Edge::EC_WORKFLOW);
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

bool BehaviourNode::canConnect_WorkflowEdge(Node *node)
{
    BehaviourNode* behaviourNode = dynamic_cast<BehaviourNode*>(node);
    if(!behaviourNode){
        return false;
    }

    return Node::canConnect_WorkflowEdge(node);
}


bool BehaviourNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

