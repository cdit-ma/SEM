#ifndef DDS_LIFESPANQOSPOLICY_H
#define DDS_LIFESPANQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_LifespanQosPolicy: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_LifespanQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_HISTORYQOSPOLICY_H


