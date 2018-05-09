#ifndef DDS_ENTITYFACTORYQOSPOLICY_H
#define DDS_ENTITYFACTORYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_EntityFactoryQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_EntityFactoryQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_ENTITYFACTORYQOSPOLICY_H


