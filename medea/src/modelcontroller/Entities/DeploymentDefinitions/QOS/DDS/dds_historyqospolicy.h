#ifndef DDS_HISTORYQOSPOLICY_H
#define DDS_HISTORYQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_HistoryQosPolicy: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_HistoryQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_HISTORYQOSPOLICY_H


