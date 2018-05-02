#ifndef DDS_LIVELINESSQOSPOLICY_H
#define DDS_LIVELINESSQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LivelinessQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_LivelinessQosPolicy(EntityFactory* factory);
	DDS_LivelinessQosPolicy();
};
#endif // DDS_LIVELINESSQOSPOLICY_H


