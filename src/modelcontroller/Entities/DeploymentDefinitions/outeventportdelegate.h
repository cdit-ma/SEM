#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "eventportassembly.h"

class EntityFactory;
class OutEventPortDelegate: public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    OutEventPortDelegate(EntityFactory& factory, bool is_temp_node);
};


#endif // OUTEVENTPORTDELEGATE_H
