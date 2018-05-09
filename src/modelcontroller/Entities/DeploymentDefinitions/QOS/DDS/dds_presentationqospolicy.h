#ifndef DDS_PRESENTATIONQOSPOLICY_H
#define DDS_PRESENTATIONQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_PresentationQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_PresentationQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_PRESENTATIONQOSPOLICY_H


