#include "workflowedge.h"
#include "../node.h"

WorkflowEdge::WorkflowEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_WORKFLOW)
{
}


WorkflowEdge::~WorkflowEdge()
{
}

QString WorkflowEdge::toString()
{
    return QString("WorkflowEdge[%1]: [" + getSource()->toString() +"] <-> [" + getDestination()->toString() + "]").arg(this->getID());
}
