#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "eventportassembly.h"

class EntityFactory;
class OutEventPortDelegate: public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPortDelegate(EntityFactory* factory);
	OutEventPortDelegate();
};


#endif // OUTEVENTPORTDELEGATE_H
