#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"


class BehaviourNode : public Node
{
public:
    BehaviourNode(bool isStart = false, bool isEnd = false, bool restrictDepth = true, Node::NODE_TYPE type = NT_NODE);
    bool isStart();
    bool isEnd();
public:
    bool canConnect(Node* attachableObject);
    virtual bool canAdoptChild(Node* child);
private:
    bool _isStart;
    bool _isEnd;
    bool _restrictDepth;
};

#endif // BEHAVIOURNODE_H
