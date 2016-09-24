#ifndef DDS_LIVELINESSQOSPOLICY_H
#define DDS_LIVELINESSQOSPOLICY_H
#include "../../../node.h"

class DDS_LivelinessQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_LivelinessQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_LIVELINESSQOSPOLICY_H


