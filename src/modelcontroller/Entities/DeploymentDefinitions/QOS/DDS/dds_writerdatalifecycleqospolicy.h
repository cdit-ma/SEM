#ifndef DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#define DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_WriterDataLifecycleQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_WriterDataLifecycleQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_WRITERDATALIFECYCLEQOSPOLICY_H


