#ifndef DDS_TIMEBASEDFILTERQOSPOLICY_H
#define DDS_TIMEBASEDFILTERQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TimeBasedFilterQosPolicy: public Node
{
    Q_OBJECT
protected:
	DDS_TimeBasedFilterQosPolicy(EntityFactory* factory);
	DDS_TimeBasedFilterQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_TIMEBASEDFILTERQOSPOLICY_H


