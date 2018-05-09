#ifndef DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#define DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_WriterDataLifecycleQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_WriterDataLifecycleQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_WRITERDATALIFECYCLEQOSPOLICY_H


