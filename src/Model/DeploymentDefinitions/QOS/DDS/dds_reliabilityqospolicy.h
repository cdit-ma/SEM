#ifndef DDS_RELIABILITYQOSPOLICY_H
#define DDS_RELIABILITYQOSPOLICY_H
#include "../../../node.h"

class DDS_ReliabilityQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_ReliabilityQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

    QList<Data*> getDefaultData();
};
#endif // DDS_RELIABILITYQOSPOLICY_H


