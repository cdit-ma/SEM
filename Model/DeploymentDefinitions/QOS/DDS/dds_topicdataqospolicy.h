#ifndef DDS_TOPICDATAQOSPOLICY_H
#define DDS_TOPICDATAQOSPOLICY_H
#include "../../../node.h"

class DDS_TopicDataQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_TopicDataQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_TOPICDATAQOSPOLICY_H


