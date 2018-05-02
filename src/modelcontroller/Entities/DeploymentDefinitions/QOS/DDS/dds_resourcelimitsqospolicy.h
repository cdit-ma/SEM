#ifndef DDS_RESOURCELIMITSQOSPOLICY_H
#define DDS_RESOURCELIMITSQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_ResourceLimitsQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_ResourceLimitsQosPolicy(EntityFactory* factory);
	DDS_ResourceLimitsQosPolicy();
};
#endif // DDS_RESOURCELIMITSQOSPOLICY_H


