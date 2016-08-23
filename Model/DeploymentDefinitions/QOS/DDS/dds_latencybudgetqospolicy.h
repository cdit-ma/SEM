#ifndef DDS_LATENCYBUDGETQOSPOLICY_H
#define DDS_LATENCYBUDGETQOSPOLICY_H
#include "../../../node.h"

class DDS_LatencyBudgetQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_LatencyBudgetQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_LATENCYBUDGETQOSPOLICY_H


