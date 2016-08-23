#ifndef DDS_READERDATALIFECYCLEQOSPOLICY_H
#define DDS_READERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"

class DDS_ReaderDataLifecycleQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_ReaderDataLifecycleQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_READERDATALIFECYCLEQOSPOLICY_H
