#ifndef DDS_HISTORYQOSPOLICY_H
#define DDS_HISTORYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_HistoryQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_HistoryQosPolicy(EntityFactory* factory);
	DDS_HistoryQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_HISTORYQOSPOLICY_H


