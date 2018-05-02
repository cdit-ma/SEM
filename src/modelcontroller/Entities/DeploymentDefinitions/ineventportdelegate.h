#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "eventportassembly.h"

class EntityFactory;
class InEventPortDelegate: public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortDelegate(EntityFactory* factory);
	InEventPortDelegate();
};

#endif // INEVENTPORTDELEGATE_H
