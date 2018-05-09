#ifndef DDS_READERDATALIFECYCLEQOSPOLICY_H
#define DDS_READERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_ReaderDataLifecycleQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_ReaderDataLifecycleQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_READERDATALIFECYCLEQOSPOLICY_H
