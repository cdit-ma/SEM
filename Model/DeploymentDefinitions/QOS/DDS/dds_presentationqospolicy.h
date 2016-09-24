#ifndef DDS_PRESENTATIONQOSPOLICY_H
#define DDS_PRESENTATIONQOSPOLICY_H
#include "../../../node.h"

class DDS_PresentationQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_PresentationQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_PRESENTATIONQOSPOLICY_H


