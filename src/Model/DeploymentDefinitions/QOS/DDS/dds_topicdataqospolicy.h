#ifndef DDS_TOPICDATAQOSPOLICY_H
#define DDS_TOPICDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TopicDataQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_TopicDataQosPolicy(EntityFactory* factory);
	DDS_TopicDataQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_TOPICDATAQOSPOLICY_H


