#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "eventportassembly.h"

class EntityFactory;
class InEventPortDelegate: public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    InEventPortDelegate(EntityFactory& factory, bool is_temp_node);
};

#endif // INEVENTPORTDELEGATE_H
