#ifndef DDS_DEADLINEQOSPOLICY_H
#define DDS_DEADLINEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DeadlineQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_DeadlineQosPolicy(EntityFactory* factory);
	DDS_DeadlineQosPolicy();
};
#endif // DDS_DEADLINEQOSPOLICY_H


