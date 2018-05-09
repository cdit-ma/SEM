#ifndef DDS_USERDATAQOSPOLICY_H
#define DDS_USERDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_UserDataQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_UserDataQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_USERDATAQOSPOLICY_H


