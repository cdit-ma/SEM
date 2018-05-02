#ifndef DDS_PRESENTATIONQOSPOLICY_H
#define DDS_PRESENTATIONQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_PresentationQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_PresentationQosPolicy(EntityFactory* factory);
	DDS_PresentationQosPolicy();
};
#endif // DDS_PRESENTATIONQOSPOLICY_H


