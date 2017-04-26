#ifndef DDS_TIMEBASEDFILTERQOSPOLICY_H
#define DDS_TIMEBASEDFILTERQOSPOLICY_H
#include "../../../node.h"

class DDS_TimeBasedFilterQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_TimeBasedFilterQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
    QList<Data*> getDefaultData();
};
#endif // DDS_TIMEBASEDFILTERQOSPOLICY_H


