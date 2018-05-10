#ifndef DDS_TIMEBASEDFILTERQOSPOLICY_H
#define DDS_TIMEBASEDFILTERQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_TimeBasedFilterQosPolicy: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_TimeBasedFilterQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_TIMEBASEDFILTERQOSPOLICY_H


