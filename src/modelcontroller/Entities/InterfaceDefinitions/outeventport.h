#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "eventport.h"

class EntityFactory;
class OutEventPort : public EventPort
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    OutEventPort(EntityFactory& factory, bool is_temp_node);
};
#endif // OUTEVENTPORT_H
