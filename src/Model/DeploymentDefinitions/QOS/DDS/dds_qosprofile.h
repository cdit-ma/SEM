#ifndef DDS_QOSPROFILE_H
#define DDS_QOSPROFILE_H
#include "../../../node.h"

class DDS_QOSProfile: public Node
{
    Q_OBJECT
public:
    DDS_QOSProfile();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_QOSPROFILE_H


