#ifndef DDS_LIFESPANQOSPOLICY_H
#define DDS_LIFESPANQOSPOLICY_H
#include "../../../node.h"

class DDS_LifespanQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_LifespanQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_HISTORYQOSPOLICY_H


