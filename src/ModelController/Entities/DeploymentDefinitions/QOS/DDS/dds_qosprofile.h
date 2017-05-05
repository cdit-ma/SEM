#ifndef DDS_QOSPROFILE_H
#define DDS_QOSPROFILE_H
#include "../../../node.h"

class EntityFactory;
class DDS_QOSProfile: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_QOSProfile(EntityFactory* factory);
	DDS_QOSProfile();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_QOSPROFILE_H


