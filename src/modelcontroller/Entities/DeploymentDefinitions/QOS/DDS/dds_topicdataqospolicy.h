#ifndef DDS_TOPICDATAQOSPOLICY_H
#define DDS_TOPICDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TopicDataQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_TopicDataQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_TOPICDATAQOSPOLICY_H


