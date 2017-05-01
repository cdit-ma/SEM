#ifndef DDS_LIFESPANQOSPOLICY_H
#define DDS_LIFESPANQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LifespanQosPolicy: public Node
{
    Q_OBJECT
protected:
	DDS_LifespanQosPolicy(EntityFactory* factory);
	DDS_LifespanQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_HISTORYQOSPOLICY_H


