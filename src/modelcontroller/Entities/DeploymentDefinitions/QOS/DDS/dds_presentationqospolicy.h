#ifndef DDS_PRESENTATIONQOSPOLICY_H
#define DDS_PRESENTATIONQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_PresentationQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_PresentationQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_PRESENTATIONQOSPOLICY_H


