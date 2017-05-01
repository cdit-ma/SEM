#ifndef DDS_GROUPDATAQOSPOLICY_H
#define DDS_GROUPDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_GroupDataQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_GroupDataQosPolicy(EntityFactory* factory);
	DDS_GroupDataQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_GROUPDATAQOSPOLICY_H


