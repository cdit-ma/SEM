#ifndef DDS_DESTINATIONORDERQOSPOLICY_H
#define DDS_DESTINATIONORDERQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DestinationOrderQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_DestinationOrderQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_DESTINATIONORDERQOSPOLICY_H


