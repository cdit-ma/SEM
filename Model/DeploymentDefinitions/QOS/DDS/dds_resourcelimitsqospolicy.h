#ifndef DDS_RESOURCELIMITSQOSPOLICY_H
#define DDS_RESOURCELIMITSQOSPOLICY_H
#include "../../../node.h"

class DDS_ResourceLimitsQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_ResourceLimitsQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_RESOURCELIMITSQOSPOLICY_H


