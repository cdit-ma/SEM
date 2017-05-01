#ifndef DDS_LIVELINESSQOSPOLICY_H
#define DDS_LIVELINESSQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LivelinessQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_LivelinessQosPolicy(EntityFactory* factory);
	DDS_LivelinessQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_LIVELINESSQOSPOLICY_H


