#ifndef DDS_TOPICDATAQOSPOLICY_H
#define DDS_TOPICDATAQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_TopicDataQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_TopicDataQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_TOPICDATAQOSPOLICY_H


