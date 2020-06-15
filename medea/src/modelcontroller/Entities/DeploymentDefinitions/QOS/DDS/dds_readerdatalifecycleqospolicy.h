#ifndef DDS_READERDATALIFECYCLEQOSPOLICY_H
#define DDS_READERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_ReaderDataLifecycleQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_ReaderDataLifecycleQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_READERDATALIFECYCLEQOSPOLICY_H
