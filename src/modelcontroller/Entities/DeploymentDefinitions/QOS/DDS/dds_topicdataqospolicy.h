#ifndef DDS_TOPICDATAQOSPOLICY_H
#define DDS_TOPICDATAQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TopicDataQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_TopicDataQosPolicy(EntityFactory* factory);
	DDS_TopicDataQosPolicy();
};
#endif // DDS_TOPICDATAQOSPOLICY_H


