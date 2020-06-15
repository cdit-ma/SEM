#ifndef DDS_DEADLINEQOSPOLICY_H
#define DDS_DEADLINEQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_DeadlineQosPolicy: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_DeadlineQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_DEADLINEQOSPOLICY_H


