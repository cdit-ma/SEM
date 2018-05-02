#ifndef DDS_LIFESPANQOSPOLICY_H
#define DDS_LIFESPANQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LifespanQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_LifespanQosPolicy(EntityFactory* factory);
	DDS_LifespanQosPolicy();
};
#endif // DDS_HISTORYQOSPOLICY_H


