#ifndef DDS_LATENCYBUDGETQOSPOLICY_H
#define DDS_LATENCYBUDGETQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LatencyBudgetQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_LatencyBudgetQosPolicy(EntityFactory* factory);
	DDS_LatencyBudgetQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_LATENCYBUDGETQOSPOLICY_H


