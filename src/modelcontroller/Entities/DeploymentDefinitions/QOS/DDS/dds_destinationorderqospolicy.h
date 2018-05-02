#ifndef DDS_DESTINATIONORDERQOSPOLICY_H
#define DDS_DESTINATIONORDERQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DestinationOrderQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_DestinationOrderQosPolicy(EntityFactory* factory);
	DDS_DestinationOrderQosPolicy();
};
#endif // DDS_DESTINATIONORDERQOSPOLICY_H


