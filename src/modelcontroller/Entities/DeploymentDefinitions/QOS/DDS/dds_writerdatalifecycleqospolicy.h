#ifndef DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#define DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_WriterDataLifecycleQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_WriterDataLifecycleQosPolicy(EntityFactory* factory);
	DDS_WriterDataLifecycleQosPolicy();
};
#endif // DDS_WRITERDATALIFECYCLEQOSPOLICY_H


