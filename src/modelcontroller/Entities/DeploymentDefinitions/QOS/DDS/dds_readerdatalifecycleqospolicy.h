#ifndef DDS_READERDATALIFECYCLEQOSPOLICY_H
#define DDS_READERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_ReaderDataLifecycleQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_ReaderDataLifecycleQosPolicy(EntityFactory* factory);
	DDS_ReaderDataLifecycleQosPolicy();
};
#endif // DDS_READERDATALIFECYCLEQOSPOLICY_H
