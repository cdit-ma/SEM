#ifndef DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#define DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_WriterDataLifecycleQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_WriterDataLifecycleQosPolicy(EntityFactory* factory);
	DDS_WriterDataLifecycleQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_WRITERDATALIFECYCLEQOSPOLICY_H


