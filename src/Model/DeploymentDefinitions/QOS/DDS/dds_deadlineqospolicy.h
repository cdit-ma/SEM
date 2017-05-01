#ifndef DDS_DEADLINEQOSPOLICY_H
#define DDS_DEADLINEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DeadlineQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_DeadlineQosPolicy(EntityFactory* factory);
	DDS_DeadlineQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_DEADLINEQOSPOLICY_H


