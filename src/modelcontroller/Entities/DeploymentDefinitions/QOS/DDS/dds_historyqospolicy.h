#ifndef DDS_HISTORYQOSPOLICY_H
#define DDS_HISTORYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_HistoryQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_HistoryQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_HISTORYQOSPOLICY_H


