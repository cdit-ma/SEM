#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"
#include <QHash>
#include <QList>
#include "../Edges/workflowedge.h"
class Parameter;


class BehaviourNode : public Node
{
    Q_OBJECT
public:
    BehaviourNode(Node::NODE_TYPE type = NT_NODE);

    void setIsWorkflowStart(bool start);
    void setIsWorkflowEnd(bool end);
    void setIsNonWorkflow(bool nonWorkflow);

    bool isWorkflowStart();
    bool isWorkflowEnd();
    bool isNonWorkflow();


    bool gotLeftWorkflowEdge();
    bool gotRightWorkflowEdge();

    WorkflowEdge* getLeftWorkflowEdge();
    WorkflowEdge* getRightWorkflowEdge();

    BehaviourNode* getRightBehaviourNode();
    BehaviourNode* getLeftBehaviourNode();

    BehaviourNode* getParentBehaviourNode();

    bool isRelatedToBehaviourNode(BehaviourNode* node);


    BehaviourNode* getStartOfWorkflowChain();
    BehaviourNode* getEndOfWorkflowChain();


    bool needEdge();

public:
    bool canConnect_WorkflowEdge(Node *node);
    virtual bool canAdoptChild(Node* child);

private:
    bool _isWorkflowStart;
    bool _isWorkflowEnd;
    bool _isNonWorkflow;
};

#endif // BEHAVIOURNODE_H
