#ifndef DDS_USERDATAQOSPOLICY_H
#define DDS_USERDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_UserDataQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_UserDataQosPolicy(EntityFactory* factory);
	DDS_UserDataQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_USERDATAQOSPOLICY_H


