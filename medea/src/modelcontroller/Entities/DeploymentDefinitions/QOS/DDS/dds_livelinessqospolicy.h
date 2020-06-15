#ifndef DDS_LIVELINESSQOSPOLICY_H
#define DDS_LIVELINESSQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_LivelinessQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_LivelinessQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_LIVELINESSQOSPOLICY_H


