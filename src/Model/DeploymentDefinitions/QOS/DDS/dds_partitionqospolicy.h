#ifndef DDS_PARTITIONQOSPOLICY_H
#define DDS_PARTITIONQOSPOLICY_H
#include "../../../node.h"

class DDS_PartitionQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_PartitionQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_PARTITIONQOSPOLICY_H


