#include "workflowedge.h"
#include "../node.h"

WorkflowEdge::WorkflowEdge(Node *src, Node *dst):Edge(src, dst, EDGE_KIND::WORKFLOW)
{
}

WorkflowEdge::WorkflowEdge(EntityFactory* factory):Edge(factory, EDGE_KIND::WORKFLOW, "Edge_Workflow"){
    auto kind = EDGE_KIND::WORKFLOW;
	QString kind_string = "Edge_Workflow";
	RegisterEdgeKind(factory, kind, kind_string, &WorkflowEdge::ConstructEdge);
}

WorkflowEdge *WorkflowEdge::ConstructEdge(Node *src, Node *dst)
{
    WorkflowEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(EDGE_KIND::WORKFLOW, dst)){
            edge = new WorkflowEdge(src, dst);
        }
    }
    return edge;
}
