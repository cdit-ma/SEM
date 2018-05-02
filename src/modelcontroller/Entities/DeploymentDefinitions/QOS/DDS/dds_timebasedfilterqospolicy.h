#ifndef DDS_TIMEBASEDFILTERQOSPOLICY_H
#define DDS_TIMEBASEDFILTERQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TimeBasedFilterQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_TimeBasedFilterQosPolicy(EntityFactory* factory);
	DDS_TimeBasedFilterQosPolicy();
};
#endif // DDS_TIMEBASEDFILTERQOSPOLICY_H


