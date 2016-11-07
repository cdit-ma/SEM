#ifndef WORKFLOWEDGE_H
#define WORKFLOWEDGE_H
#include "../edge.h"
class WorkflowEdge: public Edge{
    Q_OBJECT
protected:
    //Constructor
    WorkflowEdge(Node* src, Node* dst);

public:
    static WorkflowEdge* createWorkflowEdge(Node* src, Node* dst);

};


#endif // WORKFLOWEDGE_H
