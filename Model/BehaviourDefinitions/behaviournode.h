#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"
#include <QHash>
#include <QList>
#include "../Edges/workflowedge.h"
#include "../Edges/dataedge.h"
class Parameter;


class BehaviourNode : public Node
{
    Q_OBJECT
public:
    BehaviourNode(Node::NODE_TYPE type = NT_NODE);

    void setIsWorkflowStart(bool start);
    void setIsWorkflowEnd(bool end);
    void setIsNonWorkflow(bool nonWorkflow);

    void setIsDataInput(bool input);
    void setIsDataOutput(bool output);

    bool isWorkflowStart();
    bool isWorkflowEnd();
    bool isNonWorkflow();

    bool isDataInput();
    bool isDataOutput();

    bool gotDataInput();
    bool gotDataOutput();

    bool gotLeftWorkflowEdge();
    bool gotRightWorkflowEdge();

    WorkflowEdge* getLeftWorkflowEdge();
    WorkflowEdge* getRightWorkflowEdge();

    DataEdge* getInputDataEdge();
    DataEdge* getOutputDataEdge();

    Node* getInputData();
    Node* getOutputData();

    BehaviourNode* getRightBehaviourNode();
    BehaviourNode* getLeftBehaviourNode();

    BehaviourNode* getParentBehaviourNode();

    bool isRelatedToBehaviourNode(BehaviourNode* node);


    BehaviourNode* getStartOfWorkflowChain();
    BehaviourNode* getEndOfWorkflowChain();


    bool needEdge();

    bool compareableTypes(Node* node);

public:
    bool canConnect_WorkflowEdge(Node *node);
    bool canConnect_DataEdge(Node *node);
    virtual bool canAdoptChild(Node* child);

private:
    bool _isWorkflowStart;
    bool _isWorkflowEnd;
    bool _isNonWorkflow;
    bool _isDataInput;
    bool _isDataOutput;
};

#endif // BEHAVIOURNODE_H
