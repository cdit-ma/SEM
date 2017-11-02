#ifndef WORKFLOWEDGE_H
#define WORKFLOWEDGE_H
#include "../edge.h"

class EntityFactory;
class WorkflowEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    WorkflowEdge(EntityFactory* factory);
    WorkflowEdge(Node *src, Node *dst);
private:
    static WorkflowEdge* ConstructEdge(Node* src, Node* dst);
};
#endif // WORKFLOWEDGE_H
