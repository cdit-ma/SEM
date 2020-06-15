#ifndef DDS_QOSPROFILE_H
#define DDS_QOSPROFILE_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_QOSProfile: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_QOSProfile(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* node);
};
#endif // DDS_QOSPROFILE_H


