#ifndef DDS_GROUPDATAQOSPOLICY_H
#define DDS_GROUPDATAQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_GroupDataQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_GroupDataQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_GROUPDATAQOSPOLICY_H


