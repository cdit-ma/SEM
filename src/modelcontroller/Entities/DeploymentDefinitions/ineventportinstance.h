#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventportassembly.h"

class EntityFactory;
class InEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortInstance(EntityFactory* factory);
	InEventPortInstance();
};

#endif // INEVENTPORTINSTANCE_H
