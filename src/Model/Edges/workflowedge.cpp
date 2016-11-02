#include "workflowedge.h"
#include "../node.h"

WorkflowEdge::WorkflowEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_WORKFLOW)
{
}

WorkflowEdge *WorkflowEdge::createWorkflowEdge(Node *src, Node *dst)
{
    WorkflowEdge* edge = 0;
    if(src->canAcceptEdge(Edge::EC_WORKFLOW, dst)){
        edge = new WorkflowEdge(src, dst);
    }
    return edge;
}

