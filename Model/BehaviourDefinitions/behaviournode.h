#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"
#include <QHash>
#include <QList>
class Parameter;


class BehaviourNode : public Node
{
    Q_OBJECT
public:
    BehaviourNode(bool isStart = false, bool isEnd = false, bool restrictDepth = true, Node::NODE_TYPE type = NT_NODE);
    bool isStart();
    bool isEnd();
    bool isUnconnectable();
public:
    void setUnconnectable(bool unconnectable);

    bool canConnect_WorkflowEdge(Node *node);
    virtual bool canAdoptChild(Node* child);

private:
    bool _isStart;
    bool _isEnd;
    bool _restrictDepth;
    bool _isUnconnectable;
};

#endif // BEHAVIOURNODE_H
