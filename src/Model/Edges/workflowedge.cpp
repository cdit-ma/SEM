#include "workflowedge.h"
#include "../node.h"

WorkflowEdge::WorkflowEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_WORKFLOW)
{
}

WorkflowEdge *WorkflowEdge::createWorkflowEdge(Node *src, Node *dst)
{
    WorkflowEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(Edge::EC_WORKFLOW, dst)){
            edge = new WorkflowEdge(src, dst);
        }
    }else if(!src && !dst){
        //Allow an empty edge
        edge = new WorkflowEdge(0, 0);
    }
    return edge;
}

