#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "eventport.h"

class EntityFactory;
class InEventPort : public EventPort
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    InEventPort(EntityFactory& factory, bool is_temp_node);
};

#endif // INEVENTPORT_H
