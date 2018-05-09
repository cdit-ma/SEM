#ifndef DDS_LATENCYBUDGETQOSPOLICY_H
#define DDS_LATENCYBUDGETQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LatencyBudgetQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_LatencyBudgetQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_LATENCYBUDGETQOSPOLICY_H


