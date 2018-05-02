#ifndef DDS_USERDATAQOSPOLICY_H
#define DDS_USERDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_UserDataQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_UserDataQosPolicy(EntityFactory* factory);
	DDS_UserDataQosPolicy();
};
#endif // DDS_USERDATAQOSPOLICY_H


