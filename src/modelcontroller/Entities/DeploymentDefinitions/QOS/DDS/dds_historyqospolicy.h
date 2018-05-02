#ifndef DDS_HISTORYQOSPOLICY_H
#define DDS_HISTORYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_HistoryQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_HistoryQosPolicy(EntityFactory* factory);
	DDS_HistoryQosPolicy();
};
#endif // DDS_HISTORYQOSPOLICY_H


