#ifndef DDS_ENTITYFACTORYQOSPOLICY_H
#define DDS_ENTITYFACTORYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_EntityFactoryQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_EntityFactoryQosPolicy(EntityFactory* factory);
	DDS_EntityFactoryQosPolicy();
};
#endif // DDS_ENTITYFACTORYQOSPOLICY_H


