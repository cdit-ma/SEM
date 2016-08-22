#ifndef DDS_HISTORYQOSPOLICY_H
#define DDS_HISTORYQOSPOLICY_H
#include "../../../node.h"

class DDS_HistoryQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_HistoryQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_HISTORYQOSPOLICY_H


