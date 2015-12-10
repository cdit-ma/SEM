#ifndef WORKFLOWEDGE_H
#define WORKFLOWEDGE_H
#include "../edge.h"
class Node;

class WorkflowEdge: public Edge{
    Q_OBJECT
public:
    //Constructor
    WorkflowEdge(Node* src, Node* dst);
    ~WorkflowEdge();
    QString toString();
};


#endif // WORKFLOWEDGE_H
