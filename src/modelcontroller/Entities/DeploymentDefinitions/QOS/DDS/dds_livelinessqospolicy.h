#ifndef DDS_LIVELINESSQOSPOLICY_H
#define DDS_LIVELINESSQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LivelinessQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_LivelinessQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_LIVELINESSQOSPOLICY_H


