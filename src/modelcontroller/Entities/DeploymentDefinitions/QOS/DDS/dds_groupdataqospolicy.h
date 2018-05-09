#ifndef DDS_GROUPDATAQOSPOLICY_H
#define DDS_GROUPDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_GroupDataQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_GroupDataQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_GROUPDATAQOSPOLICY_H


