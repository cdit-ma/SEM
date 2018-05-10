#ifndef DDS_USERDATAQOSPOLICY_H
#define DDS_USERDATAQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_UserDataQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_UserDataQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_USERDATAQOSPOLICY_H


