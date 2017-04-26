#ifndef DDS_DURABILITYQOSPOLICY_H
#define DDS_DURABILITYQOSPOLICY_H
#include "../../../node.h"

class DDS_DurabilityQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_DurabilityQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
    QList<Data *> getDefaultData();
};
#endif // DDS_DURABILITYQOSPOLICY_H


