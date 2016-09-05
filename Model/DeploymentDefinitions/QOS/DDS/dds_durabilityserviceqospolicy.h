#ifndef DDS_DURABILITYSERVICEQOSPOLICY_H
#define DDS_DURABILITYSERVICEQOSPOLICY_H
#include "../../../node.h"

class DDS_DurabilityServiceQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_DurabilityServiceQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_DURABILITYSERVICEQOSPOLICY_H


