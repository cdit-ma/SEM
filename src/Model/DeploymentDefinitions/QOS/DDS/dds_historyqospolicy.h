#ifndef DDS_HISTORYQOSPOLICY_H
#define DDS_HISTORYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_HistoryQosPolicy: public Node
{
    Q_OBJECT
protected:
	DDS_HistoryQosPolicy(EntityFactory* factory);
	DDS_HistoryQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_HISTORYQOSPOLICY_H


