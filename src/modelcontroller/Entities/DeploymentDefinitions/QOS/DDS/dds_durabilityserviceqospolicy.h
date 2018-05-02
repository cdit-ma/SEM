#ifndef DDS_DURABILITYSERVICEQOSPOLICY_H
#define DDS_DURABILITYSERVICEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DurabilityServiceQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_DurabilityServiceQosPolicy(EntityFactory* factory);
	DDS_DurabilityServiceQosPolicy();
};
#endif // DDS_DURABILITYSERVICEQOSPOLICY_H


